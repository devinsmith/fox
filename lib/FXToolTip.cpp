/********************************************************************************
*                                                                               *
*                         T o o l   T i p   W i d g e t                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2024 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxmath.h"
#include "FXMutex.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXElement.h"
#include "FXArray.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXStringDictionary.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXFont.h"
#include "FXCursor.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXDCWindow.h"
#include "FXApp.h"
#include "FXToolTip.h"
#include "fxpriv.h"

/*
  Notes:
  - Initial colors are now obtained from FXApp and therefore
    from the system registry.
  - Do not assume root window is at (0,0); multi-monitor machines may
    have secondary monitor anywhere relative to primary display.
  - Text length dependent tooltip display by leonard@hipgraphics.com.
*/

#define HSPACE  4
#define VSPACE  2

using namespace FX;

/*******************************************************************************/

namespace FX {

// Map
FXDEFMAP(FXToolTip) FXToolTipMap[]={
  FXMAPFUNC(SEL_UPDATE,0,FXToolTip::onUpdate),
  FXMAPFUNC(SEL_PAINT,0,FXToolTip::onPaint),
  FXMAPFUNC(SEL_TIMEOUT,FXToolTip::ID_TIP_SHOW,FXToolTip::onTipShow),
  FXMAPFUNC(SEL_TIMEOUT,FXToolTip::ID_TIP_HIDE,FXToolTip::onTipHide),
  FXMAPFUNC(SEL_COMMAND,FXToolTip::ID_SETSTRINGVALUE,FXToolTip::onCmdSetStringValue),
  FXMAPFUNC(SEL_COMMAND,FXToolTip::ID_GETSTRINGVALUE,FXToolTip::onCmdGetStringValue),
  };


// Object implementation
FXIMPLEMENT(FXToolTip,FXShell,FXToolTipMap,ARRAYNUMBER(FXToolTipMap))


// Deserialization
FXToolTip::FXToolTip(){
  font=nullptr;
  textColor=0;
  popped=false;
  }


// Create a toplevel window
FXToolTip::FXToolTip(FXApp* a,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXShell(a,opts,x,y,w,h),label("Tooltip"){
  font=getApp()->getNormalFont();
  textColor=getApp()->getTipforeColor();
  backColor=getApp()->getTipbackColor();
  popped=false;
  }


// Tooltips do override-redirect
FXbool FXToolTip::doesOverrideRedirect() const {
  return true;
  }


// Tooltips do save-unders
FXbool FXToolTip::doesSaveUnder() const {
  return true;
  }


#ifdef WIN32
const void* FXToolTip::GetClass() const { return TEXT("FXPopup"); }
#endif


// Create window
void FXToolTip::create(){
  FXShell::create();
  font->create();
  }


// Detach window
void FXToolTip::detach(){
  FXShell::detach();
  font->detach();
  }


// Show window
void FXToolTip::show(){
  FXShell::show();
  raise();
  }


// Get default width
FXint FXToolTip::getDefaultWidth(){
  const FXchar *beg,*end;
  FXint w,tw=0;
  beg=label.text();
  if(beg){
    do{
      end=beg;
      while(*end!='\0' && *end!='\n') end++;
      if((w=font->getTextWidth(beg,end-beg))>tw) tw=w;
      beg=end+1;
      }
    while(*end!='\0');
    }
  return tw+HSPACE+HSPACE+2;
  }


// Get default height
FXint FXToolTip::getDefaultHeight(){
  const FXchar *beg,*end;
  FXint th=0;
  beg=label.text();
  if(beg){
    do{
      end=beg;
      while(*end!='\0' && *end!='\n') end++;
      th+=font->getFontHeight();
      beg=end+1;
      }
    while(*end!='\0');
    }
  return th+VSPACE+VSPACE+2;
  }


// Handle repaint
long FXToolTip::onPaint(FXObject*,FXSelector,void* ptr){
  FXEvent *ev=(FXEvent*)ptr;
  FXDCWindow dc(this,ev);
  const FXchar *beg,*end;
  FXint tx,ty;
  dc.setForeground(backColor);
  dc.fillRectangle(ev->rect.x,ev->rect.y,ev->rect.w,ev->rect.h);
  dc.setForeground(textColor);
  dc.setFont(font);
  dc.drawRectangle(0,0,width-1,height-1);
  beg=label.text();
  if(beg){
    tx=1+HSPACE;
    ty=1+VSPACE+font->getFontAscent();
    do{
      end=beg;
      while(*end!='\0' && *end!='\n') end++;
      dc.drawText(tx,ty,beg,end-beg);
      ty+=font->getFontHeight();
      beg=end+1;
      }
    while(*end!='\0');
    }
  return 1;
  }


// Place the tool tip
void FXToolTip::place(FXint x,FXint y){
  FXint rx,ry,rw,rh,px,py,w,h;
  w=getDefaultWidth();
  h=getDefaultHeight();
#ifdef WIN32
  RECT rect;
  MONITORINFO minfo;
  HMONITOR monitor;

  rect.left=x;
  rect.right=x+w;
  rect.top=y;
  rect.bottom=y+h;

  // Get monitor info if we have this API
  monitor=MonitorFromRect(&rect,MONITOR_DEFAULTTOPRIMARY);
  if(monitor){
    memset(&minfo,0,sizeof(minfo));
    minfo.cbSize=sizeof(minfo);
    GetMonitorInfo(monitor,&minfo);
    rx=minfo.rcWork.left;
    ry=minfo.rcWork.top;
    rw=minfo.rcWork.right-minfo.rcWork.left;
    rh=minfo.rcWork.bottom-minfo.rcWork.top;
    }

  // Otherwise use the work-area
  else{
    SystemParametersInfo(SPI_GETWORKAREA,sizeof(RECT),&rect,0);
    rx=rect.left;
    ry=rect.top;
    rw=rect.right-rect.left;
    rh=rect.bottom-rect.top;
    }
#else
  rx=getRoot()->getX();
  ry=getRoot()->getY();
  rw=getRoot()->getWidth();
  rh=getRoot()->getHeight();
#endif
  px=x+16-w/3;
  py=y+20;
  if(px<rx) px=rx;
  if(py<ry) py=ry;
  if(px+w>rx+rw) px=rx+rw-w;
  if(py+h+50>ry+rh){ py=y-h-10; }
  position(px,py,w,h);
  }


// Automatically place tooltip
void FXToolTip::autoplace(){
  FXint x,y; FXuint state;
  getRoot()->getCursorPosition(x,y,state);
  place(x,y);
  }


// Update tooltip based on widget under cursor
long FXToolTip::onUpdate(FXObject* sender,FXSelector sel,void* ptr){

  // Regular GUI update
  FXWindow::onUpdate(sender,sel,ptr);

  // Ask the help source for a new status text first
  FXWindow *helpsource=getApp()->getCursorWindow();
  if(helpsource && helpsource->handle(this,FXSEL(SEL_QUERY_TIP,0),nullptr)){
    if(!popped){
      popped=true;
      if(!shown()){
        getApp()->addTimeout(this,ID_TIP_SHOW,getApp()->getToolTipPause());
        return 1;
        }
      autoplace();
      }
    return 1;
    }
  getApp()->removeTimeout(this,ID_TIP_SHOW);
  popped=false;
  hide();
  return 1;
  }


// Pop the tool tip now
long FXToolTip::onTipShow(FXObject*,FXSelector,void*){
  FXTRACE((250,"%s::onTipShow %p\n",getClassName(),this));
  if(!label.empty()){
    autoplace();
    show();
    if(!(options&TOOLTIP_PERMANENT)){
      FXTime timeoutns=getApp()->getToolTipTime();
      if(options&TOOLTIP_VARIABLE){
        timeoutns=timeoutns/4+(timeoutns*label.length())/64;
        }
      getApp()->addTimeout(this,ID_TIP_HIDE,timeoutns);
      }
    }
  return 1;
  }


// Tip should hide now
long FXToolTip::onTipHide(FXObject*,FXSelector,void*){
  FXTRACE((250,"%s::onTipHide %p\n",getClassName(),this));
  hide();
  return 1;
  }


// Change value
long FXToolTip::onCmdSetStringValue(FXObject*,FXSelector,void* ptr){
  setText(*((FXString*)ptr));
  return 1;
  }


// Obtain value
long FXToolTip::onCmdGetStringValue(FXObject*,FXSelector,void* ptr){
  *((FXString*)ptr)=getText();
  return 1;
  }


// Change text
void FXToolTip::setText(const FXString& text){
  if(label!=text){
    label=text;
    recalc();
    popped=false;       // If text changes, pop it up again
    update();
    }
  }


// Change the font
void FXToolTip::setFont(FXFont *fnt){
  if(!fnt){ fxerror("%s::setFont: NULL font specified.\n",getClassName()); }
  if(font!=fnt){
    font=fnt;
    recalc();
    update();
    }
  }


// Set text color
void FXToolTip::setTextColor(FXColor clr){
  if(clr!=textColor){
    textColor=clr;
    update();
    }
  }


// Save data
void FXToolTip::save(FXStream& store) const {
  FXShell::save(store);
  store << label;
  store << font;
  store << textColor;
  }


// Load data
void FXToolTip::load(FXStream& store){
  FXShell::load(store);
  store >> label;
  store >> font;
  store >> textColor;
  }


// Destroy label
FXToolTip::~FXToolTip(){
  getApp()->removeTimeout(this,ID_TIP_SHOW);
  getApp()->removeTimeout(this,ID_TIP_HIDE);
  font=(FXFont*)-1L;
  }

}
