/********************************************************************************
*                                                                               *
*                       T A R G A  I m a g e   O b j e c t                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2001,2024 by Janusz Ganczarski.   All Rights Reserved.          *
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
#include "FXArray.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXMemoryStream.h"
#include "FXTGAImage.h"



/*
  Notes:
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// Suggested file extension
const FXchar FXTGAImage::fileExt[]="tga";


// Suggested mime type
const FXchar FXTGAImage::mimeType[]="image/targa";


// Object implementation
FXIMPLEMENT(FXTGAImage,FXImage,nullptr,0)


// Initialize
FXTGAImage::FXTGAImage(FXApp* a,const FXuchar *pix,FXuint opts,FXint w,FXint h):FXImage(a,nullptr,opts,w,h){
  if(pix){
    FXMemoryStream ms(FXStreamLoad,const_cast<FXuchar*>(pix));
    loadPixels(ms);
    }
  }


// Save pixels to stream
FXbool FXTGAImage::savePixels(FXStream& store) const {
  if(fxsaveTGA(store,data,width,height)){
    return true;
    }
  return false;
  }


// Load pixels from stream
FXbool FXTGAImage::loadPixels(FXStream& store){
  FXColor *pixels; FXint w,h;
  if(fxloadTGA(store,pixels,w,h)){
    setData(pixels,IMAGE_OWNED,w,h);
    return true;
    }
  return false;
  }


// Clean up
FXTGAImage::~FXTGAImage(){
  }

}
