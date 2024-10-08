/********************************************************************************
*                                                                               *
*                           D i s p l a y   C l a s s                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2019,2024 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "FXDisplay.h"

/*
  Notes:

*/

using namespace FX;

namespace FX {

/*******************************************************************************/

// Construct display
FXDisplay::FXDisplay():display(nullptr){
  }


// Destroy display
FXDisplay::~FXDisplay(){
  }

}
