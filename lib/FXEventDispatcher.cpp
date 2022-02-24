/********************************************************************************
*                                                                               *
*                          E v e n t   D i s p a t c h e r                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2019,2022 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxmath.h"
#include "FXAtomic.h"
#include "FXElement.h"
#include "FXHash.h"
#include "FXCallback.h"
#include "FXAutoThreadStorageKey.h"
#include "FXThread.h"
#include "FXException.h"
#include "FXReactor.h"
#include "FXReactorCore.h"
#include "FXDispatcher.h"
#include "FXEventDispatcher.h"

/*
  Notes:

  - FXEventDispatcher processes I/O handles, timers, signals, and idle activities
    just like FXDispatcher, but also processes messages/events from the display
    connection.

  - FXRawEvents from display connection are passed to dispatchEvent().

  - FXEventDispatcher::dispatch() must re-implement FXReactor::dispatch() because
    display connection is special in some sense [having to do with message queue
    management].

  - FIXME link to chain of FXEventLoop items from this class, and add the modal
    loops here.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {

// Units of time in nanoseconds
const FXTime seconds=1000000000;


// Construct event dispatcher object
FXEventDispatcher::FXEventDispatcher():display(nullptr){
  }


// Initialize dispatcher
FXbool FXEventDispatcher::init(FXptr dpy){
  if(FXDispatcher::init()){
    if(dpy){
#if !defined(WIN32)
      addHandle(ConnectionNumber((Display*)dpy),InputRead);
#endif
      display=dpy;
      return true;
      }
    }
  return false;
  }


// Initialize dispatcher
FXbool FXEventDispatcher::init(){
  if(FXDispatcher::init()){
    display=nullptr;
    return true;
    }
  return false;
  }


#if defined(_WIN32) /////////////////////////////////////////////////////////////

// Dispatch driver
FXbool FXEventDispatcher::dispatch(FXTime blocking,FXuint flags){
  if(internals){
    FXTime now,due,delay,interval;
    FXuint sig,nxt,mode,ms;
    FXInputHandle hnd;
    FXRawEvent event;

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      if(flags&DispatchTimers){
        due=nextTimeout();
        if(due<forever){
          now=FXThread::time();
          delay=due-now;
          if(delay<FXLONG(1000)){
            if(dispatchTimeout(due)) return true;       // Timer activity
            continue;
            }
          }
        }

      // Check for signal
      if(flags&DispatchSignals){
        sig=nxt=sigreceived;
        if(atomicSet(&internals->signotified[sig],0)){
          do{ nxt=(nxt+63)&63; }while(!internals->signotified[nxt] && nxt!=sig);
          sigreceived=nxt;
          if(dispatchSignal(sig)) return true;          // Signal activity
          continue;
          }
        }

      // Peek for messages.  This marks the message queue as unsignalled, so that
      // MsgWaitForMultipleObjects() would block even if there are unhandled messages.
      // The fix is to call MsgWaitForMultipleObjects() only AFTER having ascertained
      // that there are NO unhandled messages queued up.
      if(flags&DispatchEvents){
        if(PeekMessage(&event,nullptr,0,0,PM_REMOVE)){
          if(dispatchEvent(event)) return true;         // Event activity
          continue;
          }
        }

      // Check active handles
      if(0<=current && current<numhandles){
        hnd=internals->handles[current];                // Shuffle raised handle up in the list
        mode=internals->modes[current];                 // To give all handles equal play time
        nxt=(current+1)%numhandles;
        swap(internals->handles[current],internals->handles[nxt]);
        swap(internals->modes[current],internals->modes[nxt]);
        current=-1;
        if(dispatchHandle(hnd,mode,flags)) return true; // IO activity
        continue;
        }

      // Select active handles or events; don't block
      if(flags&DispatchEvents){
        current=MsgWaitForMultipleObjectsEx(numhandles,internals->handles,0,QS_ALLINPUT,MWMO_ALERTABLE);
        }
      else{
        current=WaitForMultipleObjectsEx(numhandles,internals->handles,false,0,true);
        }

      // No handles were active
      if(current==WAIT_TIMEOUT){

        // Idle callback if we're about to block
        if(flags&DispatchIdle){
          if(dispatchIdle()) return true;               // Idle activity
          }

        // We're not blocking
        if(blocking<=0) return false;

        // One more call to PeekMessage() here because the preceding idle processing
        // may have caused some more messages to be posted to the message queue.
        // A call to MsgWaitForMultipleObjects() when there are messages already in
        // the queue would block until the next message comes in.
        if(flags&DispatchEvents){
          if(PeekMessage(&event,nullptr,0,0,PM_REMOVE)){
           if(dispatchEvent(event)) return true;        // Event activity
           continue;
           }
         }

        // Indefinite wait
        ms=INFINITE;

        // If not blocking indefinitely, don't exceed maxwait time interval.
        interval=Math::imin(delay,blocking);
        if(interval<forever){
          interval=Math::imin(interval,maxwait);
          ms=(FXuint)(interval/1000000);
          }

        // Select active handles or events; wait for timeout or maximum block time
        if(flags&DispatchEvents){
          current=MsgWaitForMultipleObjectsEx(numhandles,internals->handles,ms,QS_ALLINPUT,MWMO_ALERTABLE);
          }
        else{
          current=WaitForMultipleObjectsEx(numhandles,internals->handles,false,ms,true);
          }

        // Return if there was no timeout within maximum block time
        if(current==WAIT_TIMEOUT){
          if(blocking<forever){                         // Next blocking period reduced by time already expired
            blocking-=interval;
            if(blocking<=0) return false;               // Nothing happened during blocking period!
            }
          continue;
          }
        }

      // I/O completion took place; maybe some i/o took place, causing
      // somestuff to have been changed; we must leave loop.
      if(current==WAIT_IO_COMPLETION) return false;

      // Bad stuff happened
      if(current==WAIT_FAILED || current>=WAIT_ABANDONED_0){
        throw FXFatalException("FXReactor::dispatch: error waiting on handles.");
        }
      }
    }
  return false;
  }

#elif defined(HAVE_EPOLL_CREATE1) ///////////////////////////////////////////////

// Dispatch driver
FXbool FXEventDispatcher::dispatch(FXTime blocking,FXuint flags){
  if(internals){
    FXTime now,due,delay,interval;
    FXuint sig,nxt,mode,ms;
    FXInputHandle hnd;
    FXRawEvent event;

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      if(flags&DispatchTimers){
        due=nextTimeout();
        if(due<forever){
          now=FXThread::time();
          delay=due-now;
          if(delay<FXLONG(1000)){
            if(dispatchTimeout(due)) return true;       // Timer activity
            continue;
            }
          }
        }

      // Check for signal
      if(flags&DispatchSignals){
        sig=nxt=sigreceived;
        if(atomicSet(&internals->signotified[sig],0)){
          do{ nxt=(nxt+63)&63; }while(!internals->signotified[nxt] && nxt!=sig);
          sigreceived=nxt;
          if(dispatchSignal(sig)) return true;          // Signal activity
          continue;
          }
        }

      // Handle messages
      if(flags&DispatchEvents){
        if(XEventsQueued((Display*)display,QueuedAfterFlush)){
          XNextEvent((Display*)display,&event);
// FIXME compress events
          if(dispatchEvent(event)) return true;         // Event activity
          continue;
          }
        }

      // Check active handles
      if(0<numraised){
        mode=0;
        numraised--;
        current=(current+1)%numwatched;
        hnd=internals->events[current].data.fd;
        if(internals->events[current].events&EPOLLIN){ mode|=InputRead; }
        if(internals->events[current].events&EPOLLOUT){ mode|=InputWrite; }
        if(internals->events[current].events&EPOLLERR){ mode|=InputExcept; }

        // Display connection became active
        if(ConnectionNumber((Display*)display)==hnd) continue;

        // Regular handle became active
        if(dispatchHandle(hnd,mode,flags)) return true;
        continue;
        }

      // Select active handles and check signals; don't block
      numwatched=epoll_pwait(internals->handle,internals->events,ARRAYNUMBER(internals->events),0,nullptr);

      // No active handles yet; need to wait
      if(numwatched==0){

        // Idle callback if we're about to block
        if(flags&DispatchIdle){
          if(dispatchIdle()) return true;               // Idle activity
          }

        // We're not blocking
        if(blocking<=0) return false;

        // Indefinite wait
        ms=-1;

        // If not blocking indefinitely, don't exceed maxwait time interval.
        interval=Math::imin(delay,blocking);
        if(interval<forever){
          interval=Math::imin(interval,maxwait);
          ms=(FXuint)(interval/1000000);
          }

        // Select active handles and check signals, waiting for timeout or maximum block time
        numwatched=epoll_pwait(internals->handle,internals->events,ARRAYNUMBER(internals->events),ms,nullptr);

        // Return if there was no timeout within maximum block time
        if(numwatched==0){
          if(blocking<forever){                         // Next blocking period reduced by time already expired
            blocking-=interval;
            if(blocking<=0) return false;               // Nothing happened during blocking period!
            }
          continue;
          }
        }

      // Bad stuff happened
      if(numwatched<0){
        if(errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXReactor::dispatch: error waiting on handles."); }
        continue;
        }

      // Keep track of original set
      numraised=numwatched;
      }
    }
  return false;
  }

#else ///////////////////////////////////////////////////////////////////////////

// Dispatch driver
FXbool FXEventDispatcher::dispatch(FXTime blocking,FXuint flags){
  if(internals){
    FXTime now,due,delay,interval;
    FXuint sig,nxt,mode;
    FXRawEvent event;
#if (_POSIX_C_SOURCE >= 200112L)
    struct timespec delta;
#else
    struct timeval delta;
#endif

    // Loop till we got something
    while(1){

      // Check for timeout
      delay=forever;
      if(flags&DispatchTimers){
        due=nextTimeout();
        if(due<forever){
          now=FXThread::time();
          delay=due-now;
          if(delay<FXLONG(1000)){
            if(dispatchTimeout(due)) return true;       // Timer activity
            continue;
            }
          }
        }

      // Check for signal
      if(flags&DispatchSignals){
        sig=nxt=sigreceived;
        if(atomicSet(&internals->signotified[sig],0)){
          do{ nxt=(nxt+63)&63; }while(!internals->signotified[nxt] && nxt!=sig);
          sigreceived=nxt;
          if(dispatchSignal(sig)) return true;          // Signal activity
          continue;
          }
        }

      // Handle messages
      if(flags&DispatchEvents){
        if(XEventsQueued((Display*)display,QueuedAfterFlush)){
          XNextEvent((Display*)display,&event);
// FIXME compress events
          if(dispatchEvent(event)) return true;         // Event activity
          continue;
          }
        }

      // Check active handles
      if(0<numraised){
        mode=0;
        do{
          current=(current+1)%numwatched;
          if(FD_ISSET(current,&internals->watched[0])){
            FD_CLR(current,&internals->watched[0]);
            numraised--;
            mode|=InputRead;
            }
          if(FD_ISSET(current,&internals->watched[1])){
            FD_CLR(current,&internals->watched[1]);
            numraised--;
            mode|=InputWrite;
            }
          if(FD_ISSET(current,&internals->watched[2])){
            FD_CLR(current,&internals->watched[2]);
            numraised--;
            mode|=InputExcept;
            }
          }
        while(mode==0);

        // Display connection became active
        if(ConnectionNumber((Display*)display)==current) continue;

        // Regular handle became active
        if(dispatchHandle(current,mode,flags)) return true;     // IO activity
        continue;
        }

      // Prepare handles to check
      internals->watched[0]=internals->handles[0];
      internals->watched[1]=internals->handles[1];
      internals->watched[2]=internals->handles[2];

      // Select active handles and check signals; don't block
#if (_POSIX_C_SOURCE >= 200112L)
      numraised=pselect(numhandles,&internals->watched[0],&internals->watched[1],&internals->watched[2],nullptr,nullptr);
#else
      numraised=select(numhandles,&internals->watched[0],&internals->watched[1],&internals->watched[2],nullptr);
#endif

      // No handles were active
      if(numraised==0){

        // Idle callback if we're about to block
        if(flags&DispatchIdle){
          if(dispatchIdle()) return true;               // Idle activity
          }

        // We're not blocking
        if(blocking<=0) return false;

        // Prepare handles to check
        internals->watched[0]=internals->handles[0];
        internals->watched[1]=internals->handles[1];
        internals->watched[2]=internals->handles[2];

        // Nanoseconds to wait
        interval=Math::imin(delay,blocking);
        if(interval<forever){
          interval=Math::imin(interval,maxwait);
          }

        // Select active handles and check signals, waiting for timeout or maximum block time
#if (_POSIX_C_SOURCE >= 200112L)
        delta.tv_sec=interval/seconds;
        delta.tv_nsec=(interval-seconds*delta.tv_sec);
        numraised=pselect(numhandles,&internals->watched[0],&internals->watched[1],&internals->watched[2],&delta,nullptr);
#else
        delta.tv_sec=interval/seconds;
        delta.tv_usec=(interval-seconds*delta.tv_sec)/1000;
        numraised=select(numhandles,&internals->watched[0],&internals->watched[1],&internals->watched[2],&delta);
#endif

        // Return if there was no timeout within maximum block time
        if(numraised==0){
          if(blocking<forever){                         // Next blocking period reduced by time already expired
            blocking-=interval;
            if(blocking<=0) return false;               // Nothing happened during blocking period!
            }
          continue;
          }
        }

      // Bad stuff happened
      if(numraised<0){
        if(errno!=EAGAIN && errno!=EINTR){ throw FXFatalException("FXReactor::dispatch: error waiting on handles."); }
        continue;
        }

      // Keep track of original set
      numwatched=numhandles;
      }
    }
  return false;
  }

#endif //////////////////////////////////////////////////////////////////////////


// Dispatch platform-dependent event
FXbool FXEventDispatcher::dispatchEvent(FXRawEvent& event){
  return true;
  }

/*******************************************************************************/

// Exit dispatcher
FXbool FXEventDispatcher::exit(){
  if(FXDispatcher::exit()){
    display=nullptr;
    return true;
    }
  return false;
  }


// Destroy event dispatcher object
FXEventDispatcher::~FXEventDispatcher(){
  exit();
  }

}
