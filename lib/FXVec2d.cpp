/********************************************************************************
*                                                                               *
*       D o u b l e - P r e c i s i o n   2 - E l e m e n t   V e c t o r       *
*                                                                               *
*********************************************************************************
* Copyright (C) 1994,2024 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXElement.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXVec2d.h"
#include "FXVec3d.h"


using namespace FX;

/*******************************************************************************/

namespace FX {


// Normalize vector
FXVec2d normalize(const FXVec2d& v){
  FXdouble m=v.length2();
  if(__likely(m)){ return v*Math::rsqrt(m); }
  return v;
  }


// Save vector to a stream
FXStream& operator<<(FXStream& store,const FXVec2d& v){
  store << v.x << v.y;
  return store;
  }


// Load vector from a stream
FXStream& operator>>(FXStream& store,FXVec2d& v){
  store >> v.x >> v.y;
  return store;
  }

}
