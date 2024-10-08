/********************************************************************************
*                                                                               *
*                          C o l o r   S e l e c t o r                          *
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
#include "fxkeys.h"
#include "FXMutex.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXColors.h"
#include "FXElement.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXStringDictionary.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXDCWindow.h"
#include "FXObjectList.h"
#include "FXApp.h"
#include "FXFont.h"
#include "FXIcon.h"
#include "FXGIFIcon.h"
#include "FXWindow.h"
#include "FXFrame.h"
#include "FXSeparator.h"
#include "FXLabel.h"
#include "FXColorBar.h"
#include "FXColorWell.h"
#include "FXColorWheel.h"
#include "FXColorRing.h"
#include "FXColorList.h"
#include "FXTextField.h"
#include "FXButton.h"
#include "FXPicker.h"
#include "FXComposite.h"
#include "FXPacker.h"
#include "FXTabItem.h"
#include "FXTabBook.h"
#include "FXHorizontalFrame.h"
#include "FXVerticalFrame.h"
#include "FXMatrix.h"
#include "FXShell.h"
#include "FXScrollBar.h"
#include "FXSlider.h"
#include "FXList.h"
#include "FXColorSelector.h"
#include "icons.h"

/*
  Notes:
  - Custom colors in the twentyfour wells are saved to registry for next time you
    run the program.

  - HSV Model:

      green          yellow
            o-----o
           /       \
          /  white  \
    cyan o     o     o red
          \         /
           \       /
            o-----o
      blue           magenta

  - Perhaps this panel should send color change messages to active
    colorwell widget?

  - New default palette of colors; basic grey levels, primary colors,
    darker colors, and pastel colors.
*/

// Is color pure gray
#define FXISGREY(rgba) (FXREDVAL(rgba)==FXGREENVAL(rgba) && FXREDVAL(rgba)==FXBLUEVAL(rgba))

using namespace FX;

/*******************************************************************************/

namespace FX {


/*******************************************************************************/

// Map
FXDEFMAP(FXColorSelector) FXColorSelectorMap[]={
  FXMAPFUNC(SEL_UPDATE,FXColorSelector::ID_ALPHA_TEXT,FXColorSelector::onUpdAlphaText),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_ALPHA_TEXT,FXColorSelector::onCmdAlphaText),
  FXMAPFUNC(SEL_UPDATE,FXColorSelector::ID_ALPHA_LABEL,FXColorSelector::onUpdAlphaLabel),
  FXMAPFUNC(SEL_UPDATE,FXColorSelector::ID_ALPHA_SLIDER,FXColorSelector::onUpdAlphaSlider),
  FXMAPFUNC(SEL_CHANGED,FXColorSelector::ID_ALPHA_SLIDER,FXColorSelector::onCmdAlphaSlider),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_ALPHA_SLIDER,FXColorSelector::onCmdAlphaSlider),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_RGB_RED_TEXT,FXColorSelector::ID_RGB_BLUE_TEXT,FXColorSelector::onUpdRGBText),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_HSV_HUE_TEXT,FXColorSelector::ID_HSV_VALUE_TEXT,FXColorSelector::onUpdHSVText),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_CMY_CYAN_TEXT,FXColorSelector::ID_CMY_YELLOW_TEXT,FXColorSelector::onUpdCMYText),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_RGB_RED_TEXT,FXColorSelector::ID_RGB_BLUE_TEXT,FXColorSelector::onCmdRGBText),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_HSV_HUE_TEXT,FXColorSelector::ID_HSV_VALUE_TEXT,FXColorSelector::onCmdHSVText),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_CMY_CYAN_TEXT,FXColorSelector::ID_CMY_YELLOW_TEXT,FXColorSelector::onCmdCMYText),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_RGB_RED_SLIDER,FXColorSelector::ID_RGB_BLUE_SLIDER,FXColorSelector::onUpdRGBSlider),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_HSV_HUE_SLIDER,FXColorSelector::ID_HSV_VALUE_SLIDER,FXColorSelector::onUpdHSVSlider),
  FXMAPFUNCS(SEL_UPDATE,FXColorSelector::ID_CMY_CYAN_SLIDER,FXColorSelector::ID_CMY_YELLOW_SLIDER,FXColorSelector::onUpdCMYSlider),
  FXMAPFUNCS(SEL_CHANGED,FXColorSelector::ID_RGB_RED_SLIDER,FXColorSelector::ID_RGB_BLUE_SLIDER,FXColorSelector::onCmdRGBSlider),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_RGB_RED_SLIDER,FXColorSelector::ID_RGB_BLUE_SLIDER,FXColorSelector::onCmdRGBSlider),
  FXMAPFUNCS(SEL_CHANGED,FXColorSelector::ID_HSV_HUE_SLIDER,FXColorSelector::ID_HSV_VALUE_SLIDER,FXColorSelector::onCmdHSVSlider),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_HSV_HUE_SLIDER,FXColorSelector::ID_HSV_VALUE_SLIDER,FXColorSelector::onCmdHSVSlider),
  FXMAPFUNCS(SEL_CHANGED,FXColorSelector::ID_CMY_CYAN_SLIDER,FXColorSelector::ID_CMY_YELLOW_SLIDER,FXColorSelector::onCmdCMYSlider),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_CMY_CYAN_SLIDER,FXColorSelector::ID_CMY_YELLOW_SLIDER,FXColorSelector::onCmdCMYSlider),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_WELL_CHANGED,FXColorSelector::onCmdWell),
  FXMAPFUNC(SEL_CHANGED,FXColorSelector::ID_WELL_CHANGED,FXColorSelector::onChgWell),
  FXMAPFUNCS(SEL_COMMAND,FXColorSelector::ID_CUSTOM_FIRST,FXColorSelector::ID_CUSTOM_LAST,FXColorSelector::onCmdCustomWell),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_COLOR_LIST,FXColorSelector::onCmdList),
  FXMAPFUNC(SEL_UPDATE,FXColorSelector::ID_DIAL_WHEEL,FXColorSelector::onUpdWheel),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_DIAL_WHEEL,FXColorSelector::onCmdWheel),
  FXMAPFUNC(SEL_CHANGED,FXColorSelector::ID_DIAL_WHEEL,FXColorSelector::onCmdWheel),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_COLORPICK,FXColorSelector::onCmdColorPick),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_SETVALUE,FXColorSelector::onCmdSetValue),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_SETINTVALUE,FXColorSelector::onCmdSetIntValue),
  FXMAPFUNC(SEL_COMMAND,FXColorSelector::ID_GETINTVALUE,FXColorSelector::onCmdGetIntValue),
  };


// Object implementation
FXIMPLEMENT(FXColorSelector,FXPacker,FXColorSelectorMap,ARRAYNUMBER(FXColorSelectorMap))


/*******************************************************************************/

// Sort items based on color
static FXint hueSort(const FXListItem* a,const FXListItem* b){
  FXColor colora=((const FXColorItem*)a)->getColor();
  FXColor colorb=((const FXColorItem*)b)->getColor();
  FXfloat ca[3],cb[3];
  fxrgb_to_hsl(ca[0],ca[1],ca[2],0.003921568627f*FXREDVAL(colora),0.003921568627f*FXGREENVAL(colora),0.003921568627f*FXBLUEVAL(colora));
  fxrgb_to_hsl(cb[0],cb[1],cb[2],0.003921568627f*FXREDVAL(colorb),0.003921568627f*FXGREENVAL(colorb),0.003921568627f*FXBLUEVAL(colorb));
  if(ca[2]!=cb[2]) return FXSGN(cb[2]-ca[2]);
  if(ca[1]!=cb[1]) return FXSGN(ca[1]-cb[1]);
  if(ca[0]!=cb[0]) return FXSGN(cb[0]-ca[0]);
  return 0;
  }


// Separator item
FXColorSelector::FXColorSelector(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXPacker(p,opts,x,y,w,h){
  FXLabel *label;
  target=tgt;
  message=sel;

  // Buttons
  FXHorizontalFrame *buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
  accept=new FXButton(buttons,tr("&Accept"),nullptr,nullptr,0,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,20,20);
  cancel=new FXButton(buttons,tr("&Cancel"),nullptr,nullptr,0,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,20,20);

  // Separator
  new FXHorizontalSeparator(this,SEPARATOR_RIDGE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  // Icons
  eyedropicon=new FXGIFIcon(getApp(),eyedrop);
  dialmodeicon=new FXGIFIcon(getApp(),dialmode);
  rgbmodeicon=new FXGIFIcon(getApp(),rgbmode);
  hsvmodeicon=new FXGIFIcon(getApp(),hsvmode);
  cmymodeicon=new FXGIFIcon(getApp(),cmymode);
  txtmodeicon=new FXGIFIcon(getApp(),listmode);

  // Wells with custom colors
  FXHorizontalFrame *colors=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0, 0,0);
  colorwells[ 0]=new FXColorWell(colors,FXRGBA(255,255,255,255),this,ID_CUSTOM_FIRST+ 0,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 1]=new FXColorWell(colors,FXRGBA(204,204,204,255),this,ID_CUSTOM_FIRST+ 1,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 2]=new FXColorWell(colors,FXRGBA(153,153,153,255),this,ID_CUSTOM_FIRST+ 2,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 3]=new FXColorWell(colors,FXRGBA(102,102,102,255),this,ID_CUSTOM_FIRST+ 3,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 4]=new FXColorWell(colors,FXRGBA( 51, 51, 51,255),this,ID_CUSTOM_FIRST+ 4,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 5]=new FXColorWell(colors,FXRGBA(  0,  0,  0,255),this,ID_CUSTOM_FIRST+ 5,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 6]=new FXColorWell(colors,FXRGBA(255,  0,  0,255),this,ID_CUSTOM_FIRST+ 6,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 7]=new FXColorWell(colors,FXRGBA(  0,255,  0,255),this,ID_CUSTOM_FIRST+ 7,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 8]=new FXColorWell(colors,FXRGBA(  0,  0,255,255),this,ID_CUSTOM_FIRST+ 8,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[ 9]=new FXColorWell(colors,FXRGBA(  0,255,255,255),this,ID_CUSTOM_FIRST+ 9,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[10]=new FXColorWell(colors,FXRGBA(255,255,  0,255),this,ID_CUSTOM_FIRST+10,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[11]=new FXColorWell(colors,FXRGBA(255,  0,255,255),this,ID_CUSTOM_FIRST+11,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[12]=new FXColorWell(colors,FXRGBA(255,165,  0,255),this,ID_CUSTOM_FIRST+12,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[13]=new FXColorWell(colors,FXRGBA(153,  0,  0,255),this,ID_CUSTOM_FIRST+13,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[14]=new FXColorWell(colors,FXRGBA(  0,153,  0,255),this,ID_CUSTOM_FIRST+14,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[15]=new FXColorWell(colors,FXRGBA(  0,  0,153,255),this,ID_CUSTOM_FIRST+15,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[16]=new FXColorWell(colors,FXRGBA(  0,153,153,255),this,ID_CUSTOM_FIRST+16,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[17]=new FXColorWell(colors,FXRGBA(153,153,  0,255),this,ID_CUSTOM_FIRST+17,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[18]=new FXColorWell(colors,FXRGBA(153,  0,153,255),this,ID_CUSTOM_FIRST+18,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[19]=new FXColorWell(colors,FXRGBA(255,175,175,255),this,ID_CUSTOM_FIRST+19,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[20]=new FXColorWell(colors,FXRGBA(175,255,175,255),this,ID_CUSTOM_FIRST+20,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[21]=new FXColorWell(colors,FXRGBA(175,175,255,255),this,ID_CUSTOM_FIRST+21,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[22]=new FXColorWell(colors,FXRGBA(175,255,255,255),this,ID_CUSTOM_FIRST+22,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);
  colorwells[23]=new FXColorWell(colors,FXRGBA(255,255,175,255),this,ID_CUSTOM_FIRST+23,COLORWELL_SOURCEONLY|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0);

  // Main part
  FXHorizontalFrame *main=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXVerticalFrame *wellframe=new FXVerticalFrame(main,LAYOUT_FILL_Y,0,0,0,0, 0,0,10,0,0,2);

  // Color sucker
  new FXPicker(wellframe,tr("\tPick color"),eyedropicon,this,ID_COLORPICK,JUSTIFY_CENTER_X|JUSTIFY_CENTER_Y|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,56,32,0,0,0,0);

  // Main color well
  well=new FXColorWell(wellframe,FXRGBA(255,255,255,255),this,ID_WELL_CHANGED,COLORWELL_SOURCEONLY|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|FRAME_SUNKEN|FRAME_THICK, 0,0,64,0);

  // Tab book with switchable panels
  panels=new FXTabBook(main,nullptr,0,TABBOOK_TOPTABS|LAYOUT_FILL_Y|LAYOUT_FILL_X);

  // HSV Dial Mode
  new FXTabItem(panels,tr("\tHue, Saturation, Value"),dialmodeicon,TAB_TOP_NORMAL,0,0,0,0, 6,6,0,0);

    // Color wheel
    FXHorizontalFrame *dialblock=new FXHorizontalFrame(panels,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,15,15,5,5, 5,8);

    wheel=new FXColorRing(dialblock,this,ID_DIAL_WHEEL,LAYOUT_CENTER_Y|LAYOUT_FILL_Y|LAYOUT_FILL_X|JUSTIFY_LEFT|JUSTIFY_CENTER_Y,0,0,0,0,1,1,1,1);

  // RGB Mode
  new FXTabItem(panels,tr("\tRed, Green, Blue"),rgbmodeicon,TAB_TOP_NORMAL,0,0,0,0, 6,6,0,0);

    // RGB Sliders
    FXMatrix *rgbblock=new FXMatrix(panels,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);

    // Red
    new FXLabel(rgbblock,tr("&Red:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    rgbatext[0]=new FXTextField(rgbblock,5,this,FXColorSelector::ID_RGB_RED_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    rgbaslider[0]=new FXSlider(rgbblock,this,FXColorSelector::ID_RGB_RED_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Green slider
    new FXLabel(rgbblock,tr("&Green:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    rgbatext[1]=new FXTextField(rgbblock,5,this,FXColorSelector::ID_RGB_GREEN_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    rgbaslider[1]=new FXSlider(rgbblock,this,FXColorSelector::ID_RGB_GREEN_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Blue slider
    new FXLabel(rgbblock,tr("&Blue:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    rgbatext[2]=new FXTextField(rgbblock,5,this,FXColorSelector::ID_RGB_BLUE_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    rgbaslider[2]=new FXSlider(rgbblock,this,FXColorSelector::ID_RGB_BLUE_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Alpha slider
    label=new FXLabel(rgbblock,tr("&Alpha:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    rgbatext[3]=new FXTextField(rgbblock,5,this,FXColorSelector::ID_ALPHA_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    rgbaslider[3]=new FXSlider(rgbblock,this,FXColorSelector::ID_ALPHA_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    label->setTarget(this);
    label->setSelector(ID_ALPHA_LABEL);

    // Set ranges and increment
    rgbaslider[0]->setRange(0,255);
    rgbaslider[1]->setRange(0,255);
    rgbaslider[2]->setRange(0,255);
    rgbaslider[3]->setRange(0,255);

  // HSV Mode
  new FXTabItem(panels,tr("\tHue, Saturation, Value"),hsvmodeicon,TAB_TOP_NORMAL,0,0,0,0, 6,6,0,0);

    // RGB Sliders
    FXMatrix *hsvblock=new FXMatrix(panels,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);

    // Hue Slider
    new FXLabel(hsvblock,tr("Hue:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    hsvatext[0]=new FXTextField(hsvblock,5,this,FXColorSelector::ID_HSV_HUE_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    hsvaslider[0]=new FXSlider(hsvblock,this,FXColorSelector::ID_HSV_HUE_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Saturation slider
    new FXLabel(hsvblock,tr("Saturation:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    hsvatext[1]=new FXTextField(hsvblock,5,this,FXColorSelector::ID_HSV_SATURATION_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    hsvaslider[1]=new FXSlider(hsvblock,this,FXColorSelector::ID_HSV_SATURATION_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Value slider
    new FXLabel(hsvblock,tr("Value:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    hsvatext[2]=new FXTextField(hsvblock,5,this,FXColorSelector::ID_HSV_VALUE_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    hsvaslider[2]=new FXSlider(hsvblock,this,FXColorSelector::ID_HSV_VALUE_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Alpha slider
    label=new FXLabel(hsvblock,tr("Alpha:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    hsvatext[3]=new FXTextField(hsvblock,5,this,FXColorSelector::ID_ALPHA_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    hsvaslider[3]=new FXSlider(hsvblock,this,FXColorSelector::ID_ALPHA_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    label->setTarget(this);
    label->setSelector(ID_ALPHA_LABEL);

    // Set ranges
    hsvaslider[0]->setRange(0,360);
    hsvaslider[1]->setRange(0,1000);
    hsvaslider[2]->setRange(0,1000);
    hsvaslider[3]->setRange(0,255);

  // CMY Mode
  new FXTabItem(panels,tr("\tCyan, Magenta, Yellow"),cmymodeicon,TAB_TOP_NORMAL,0,0,0,0, 6,6,0,0);

    // RGB Sliders
    FXMatrix *cmyblock=new FXMatrix(panels,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);

    // Cyan Slider
    new FXLabel(cmyblock,tr("Cyan:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    cmytext[0]=new FXTextField(cmyblock,5,this,FXColorSelector::ID_CMY_CYAN_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    cmyslider[0]=new FXSlider(cmyblock,this,FXColorSelector::ID_CMY_CYAN_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Magenta slider
    new FXLabel(cmyblock,tr("Magenta:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    cmytext[1]=new FXTextField(cmyblock,5,this,FXColorSelector::ID_CMY_MAGENTA_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    cmyslider[1]=new FXSlider(cmyblock,this,FXColorSelector::ID_CMY_MAGENTA_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Yellow slider
    new FXLabel(cmyblock,tr("Yellow:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    cmytext[2]=new FXTextField(cmyblock,5,this,FXColorSelector::ID_CMY_YELLOW_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    cmyslider[2]=new FXSlider(cmyblock,this,FXColorSelector::ID_CMY_YELLOW_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    // Alpha slider
    label=new FXLabel(cmyblock,tr("Alpha:"),nullptr,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
    cmytext[3]=new FXTextField(cmyblock,5,this,FXColorSelector::ID_ALPHA_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
    cmyslider[3]=new FXSlider(cmyblock,this,FXColorSelector::ID_ALPHA_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15);

    label->setTarget(this);
    label->setSelector(ID_ALPHA_LABEL);

    // Set ranges
    cmyslider[0]->setRange(0,255);
    cmyslider[1]->setRange(0,255);
    cmyslider[2]->setRange(0,255);
    cmyslider[3]->setRange(0,255);

  // Named Color Mode
  new FXTabItem(panels,tr("\tBy Name"),txtmodeicon,TAB_TOP_NORMAL,0,0,0,0, 6,6,0,0);

    // Name list
    FXHorizontalFrame *outer=new FXHorizontalFrame(panels,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X);
    FXHorizontalFrame *frame=new FXHorizontalFrame(outer,LAYOUT_FILL_Y|LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, 0,0,0,0);
    list=new FXColorList(frame,this,ID_COLOR_LIST,LAYOUT_FILL_Y|LAYOUT_FILL_X|LIST_BROWSESELECT);
    list->setNumVisible(8);
    list->setSortFunc(hueSort);

    // Add color names
    for(FXuint i=0; i<ARRAYNUMBER(colorName); i++){
      list->appendItem(tr(colorName[i]),colorValue[i]);
      }
    list->sortItems();

  // Init RGBA
  rgba[0]=0.0f;
  rgba[1]=0.0f;
  rgba[2]=0.0f;
  rgba[3]=1.0f;

  // Init HSVA
  hsva[0]=360.0f;
  hsva[1]=0.0f;
  hsva[2]=0.0f;
  hsva[3]=1.0f;

  // Reflect color in well
  updateWell();

  // Initial focus on accept button
  accept->setFocus();
  }

/*******************************************************************************/

// Set color from message
long FXColorSelector::onCmdSetValue(FXObject*,FXSelector,void* ptr){
  setRGBA((FXColor)(FXuval)ptr);
  return 1;
  }


// Update color selector from a message
long FXColorSelector::onCmdSetIntValue(FXObject*,FXSelector,void* ptr){
  setRGBA(*((FXColor*)ptr));
  return 1;
  }


// Obtain value from color selector
long FXColorSelector::onCmdGetIntValue(FXObject*,FXSelector,void* ptr){
  *((FXColor*)ptr)=getRGBA();
  return 1;
  }

/*******************************************************************************/

// ALPHA

// Update well from Alpha slider
long FXColorSelector::onCmdAlphaSlider(FXObject* sender,FXSelector sel,void*){
  FXint value;
  sender->handle(this,FXSEL(SEL_COMMAND,ID_GETINTVALUE),(void*)&value);
  hsva[3]=rgba[3]=0.003921568627f*value;
  updateWell();
  if(target) target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update well from Alpha text fields
long FXColorSelector::onCmdAlphaText(FXObject* sender,FXSelector,void*){
  FXdouble value;
  sender->handle(this,FXSEL(SEL_COMMAND,ID_GETREALVALUE),(void*)&value);
  hsva[3]=rgba[3]=0.003921568627f*(FXfloat)value;
  updateWell();
  if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update Alpha text fields
long FXColorSelector::onUpdAlphaText(FXObject* sender,FXSelector,void*){
  FXString value;
  if(isOpaqueOnly()){
    sender->handle(this,FXSEL(SEL_COMMAND,ID_HIDE),nullptr);
    }
  else{
    value.fromFloat(255.0f*rgba[3],1,0);
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&value);
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SHOW),nullptr);
    }
  return 1;
  }


// Update Alpha sliders
long FXColorSelector::onUpdAlphaSlider(FXObject* sender,FXSelector,void*){
  if(isOpaqueOnly()){
    sender->handle(this,FXSEL(SEL_COMMAND,ID_HIDE),nullptr);
    }
  else{
    FXint value=(FXint)(255.0f*rgba[3]);
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SETINTVALUE),(void*)&value);
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SHOW),nullptr);
    }
  return 1;
  }


// Update Alpha Labels
long FXColorSelector::onUpdAlphaLabel(FXObject* sender,FXSelector,void*){
  sender->handle(this,isOpaqueOnly()?FXSEL(SEL_COMMAND,ID_HIDE):FXSEL(SEL_COMMAND,ID_SHOW),nullptr);
  return 1;
  }


/*******************************************************************************/


// Wheel

// Update well from wheel
long FXColorSelector::onCmdWheel(FXObject*,FXSelector sel,void*){
  hsva[0]=wheel->getHue();
  hsva[1]=wheel->getSat();
  hsva[2]=wheel->getVal();
  fxhsv_to_rgb(rgba[0],rgba[1],rgba[2],hsva[0],hsva[1],hsva[2]);
  updateWell();
  if(target) target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update wheel
long FXColorSelector::onUpdWheel(FXObject*,FXSelector,void*){
  wheel->setHue(hsva[0]);
  wheel->setSat(hsva[1]);
  wheel->setVal(hsva[2]);
  return 1;
  }


/*******************************************************************************/


// RGB

// Update well from RGB slider
long FXColorSelector::onCmdRGBSlider(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_RGB_RED_SLIDER;
  rgba[which]=0.003921568627f*rgbaslider[which]->getValue();
  fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
  updateWell();
  if(target) target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update well from RGB text fields
long FXColorSelector::onCmdRGBText(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_RGB_RED_TEXT;
  rgba[which]=0.003921568627f*rgbatext[which]->getText().toFloat();
  fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
  updateWell();
  if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update RGB text fields
long FXColorSelector::onUpdRGBText(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_RGB_RED_TEXT;
  rgbatext[which]->setText(FXString::value(255.0f*rgba[which],1,0));
  return 1;
  }


// Update RGB sliders
long FXColorSelector::onUpdRGBSlider(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_RGB_RED_SLIDER;
  rgbaslider[which]->setValue((FXint)(255.0f*rgba[which]));
  return 1;
  }


/*******************************************************************************/


// HSV

// Update well from HSV sliders
long FXColorSelector::onCmdHSVSlider(FXObject*,FXSelector sel,void*){
  const FXfloat factor[3]={1.0f,0.001f,0.001f};
  FXint which=FXSELID(sel)-ID_HSV_HUE_SLIDER;
  hsva[which]=factor[which]*hsvaslider[which]->getValue();
  fxhsv_to_rgb(rgba[0],rgba[1],rgba[2],hsva[0],hsva[1],hsva[2]);
  updateWell();
  if(target) target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update well from HSV text fields
long FXColorSelector::onCmdHSVText(FXObject*,FXSelector sel,void*){
  const FXfloat factor[3]={1.0f,0.01f,0.01f};
  FXint which=FXSELID(sel)-ID_HSV_HUE_TEXT;
  hsva[which]=factor[which]*hsvatext[which]->getText().toFloat();
  fxhsv_to_rgb(rgba[0],rgba[1],rgba[2],hsva[0],hsva[1],hsva[2]);
  updateWell();
  if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update HSV text fields
long FXColorSelector::onUpdHSVText(FXObject*,FXSelector sel,void*){
  const FXfloat factor[3]={1.0f,100.0f,100.0f};
  FXint which=FXSELID(sel)-ID_HSV_HUE_TEXT;
  hsvatext[which]->setText(FXString::value(hsva[which]*factor[which],1,0));
  return 1;
  }


// Update HSV sliders
long FXColorSelector::onUpdHSVSlider(FXObject*,FXSelector sel,void*){
  const FXfloat factor[3]={1.0f,1000.0f,1000.0f};
  FXint which=FXSELID(sel)-ID_HSV_HUE_SLIDER;
  hsvaslider[which]->setValue((FXint)(hsva[which]*factor[which]));
  return 1;
  }

/*******************************************************************************/


// CMY

long FXColorSelector::onCmdCMYSlider(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_CMY_CYAN_SLIDER;
  FXfloat val=0.003921568627f*cmyslider[which]->getValue();
  rgba[which]=1.0f-val;
  fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
  hsva[3]=rgba[3];
  updateWell();
  if(target) target->tryHandle(this,FXSEL(FXSELTYPE(sel),message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


long FXColorSelector::onCmdCMYText(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_CMY_CYAN_TEXT;
  FXfloat val=0.003921568627f*cmytext[which]->getText().toFloat();
  rgba[which]=1.0f-val;
  fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
  hsva[3]=rgba[3];
  updateWell();
  if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


long FXColorSelector::onUpdCMYText(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_CMY_CYAN_TEXT;
  FXfloat val=255.0f-255.0f*rgba[which];
  cmytext[which]->setText(FXString::value(val,1,0));
  return 1;
  }


long FXColorSelector::onUpdCMYSlider(FXObject*,FXSelector sel,void*){
  FXint which=FXSELID(sel)-ID_CMY_CYAN_SLIDER;
  FXint val=(FXint)(255.0f-255.0f*rgba[which]);
  cmyslider[which]->setValue(val);
  return 1;
  }


/*******************************************************************************/


// Color picker; note, dc released prior to callback
long FXColorSelector::onCmdColorPick(FXObject*,FXSelector,void* ptr){
  FXColor color;
  {
    FXDCWindow dc(getRoot());
    color=dc.readPixel(((FXPoint*)ptr)->x,((FXPoint*)ptr)->y);
  }
  setRGBA(color,true);
  return 1;
  }


/*******************************************************************************/


// Dropped color in main well
long FXColorSelector::onChgWell(FXObject*,FXSelector,void* ptr){
  FXColor color=(FXColor)(FXuval)ptr;
  if(isOpaqueOnly()) color|=FXRGBA(0,0,0,255);
  rgba[0]=0.003921568627f*FXREDVAL(color);
  rgba[1]=0.003921568627f*FXGREENVAL(color);
  rgba[2]=0.003921568627f*FXBLUEVAL(color);
  rgba[3]=0.003921568627f*FXALPHAVAL(color);
  fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
  hsva[3]=rgba[3];
  return 1;
  }


// Command from main well
long FXColorSelector::onCmdWell(FXObject*,FXSelector,void*){
  if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }


// Update main well
void FXColorSelector::updateWell(){
  well->setRGBA(FXRGBA((int)(rgba[0]*255.0f),(int)(rgba[1]*255.0f),(int)(rgba[2]*255.0f),(int)(rgba[3]*255.0f)));
  }

/*******************************************************************************/

// Clicked on color in list
long FXColorSelector::onCmdList(FXObject*,FXSelector,void* ptr){
  setRGBA(list->getItemColor((FXint)(FXuval)ptr),true);
  return 1;
  }

/*******************************************************************************/

// Custom well clicked
long FXColorSelector::onCmdCustomWell(FXObject*,FXSelector,void* ptr){
  setRGBA((FXColor)(FXuval)ptr,true);
  return 1;
  }

/*******************************************************************************/


// Change RGBA color
void FXColorSelector::setRGBA(FXColor color,FXbool notify){
  if(isOpaqueOnly()) color|=FXRGBA(0,0,0,255);
  if(color!=well->getRGBA()){
    rgba[0]=0.003921568627f*FXREDVAL(color);
    rgba[1]=0.003921568627f*FXGREENVAL(color);
    rgba[2]=0.003921568627f*FXBLUEVAL(color);
    rgba[3]=0.003921568627f*FXALPHAVAL(color);
    fxrgb_to_hsv(hsva[0],hsva[1],hsva[2],rgba[0],rgba[1],rgba[2]);
    hsva[3]=rgba[3];
    well->setRGBA(color);
    if(target && notify) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)color);
    }
  }


// Retrieve RGBA color
FXColor FXColorSelector::getRGBA() const {
  return well->getRGBA();
  }


// Change active panel
void FXColorSelector::setActivePanel(FXint pnl){
  panels->setCurrent(pnl);
  }


// Return active panel
FXint FXColorSelector::getActivePanel() const {
  return panels->getCurrent();
  }


// Change well color
void FXColorSelector::setWellColor(FXint w,FXColor clr){
  colorwells[w]->setRGBA(clr);
  }


// Return well color
FXColor FXColorSelector::getWellColor(FXint w) const {
  return colorwells[w]->getRGBA();
  }


// Change opaque only mode
void FXColorSelector::setOpaqueOnly(FXbool opaque){
  if(opaque){
    well->setOpaqueOnly(true);
    setRGBA(well->getRGBA() | FXRGBA(0,0,0,255));
    }
  else{
    well->setOpaqueOnly(false);
    }
  }


// Return true if only opaque colors allowed
FXbool FXColorSelector::isOpaqueOnly() const {
  return well->isOpaqueOnly();
  }

/*******************************************************************************/

// Save data
void FXColorSelector::save(FXStream& store) const {
  FXPacker::save(store);
  store << panels;
  store << well;
  store << list;
  store << accept;
  store << cancel;
  store << dialmodeicon;
  store << rgbmodeicon;
  store << hsvmodeicon;
  store << cmymodeicon;
  store << txtmodeicon;
  store << wheel;
  store << rgbaslider[0] << rgbaslider[1] << rgbaslider[2] << rgbaslider[3];
  store << hsvaslider[0] << hsvaslider[1] << hsvaslider[2] << hsvaslider[3];
  store << cmyslider[0]  << cmyslider[1]  << cmyslider[2]  << cmyslider[3];
  store << rgbatext[0] << rgbatext[1] << rgbatext[2] << rgbatext[3];
  store << hsvatext[0] << hsvatext[1] << hsvatext[2] << hsvatext[3];
  store << cmytext[0]  << cmytext[1]  << cmytext[2]  << cmytext[3];
  store << colorwells[0]  << colorwells[1]  << colorwells[2]  << colorwells[3];
  store << colorwells[4]  << colorwells[5]  << colorwells[6]  << colorwells[7];
  store << colorwells[8]  << colorwells[9]  << colorwells[10] << colorwells[11];
  store << colorwells[12] << colorwells[13] << colorwells[14] << colorwells[15];
  store << colorwells[16] << colorwells[17] << colorwells[18] << colorwells[19];
  store << colorwells[20] << colorwells[21] << colorwells[22] << colorwells[23];
  store.save(rgba,4);
  store.save(hsva,4);
  }


// Load data
void FXColorSelector::load(FXStream& store){
  FXPacker::load(store);
  store >> panels;
  store >> well;
  store >> list;
  store >> accept;
  store >> cancel;
  store >> dialmodeicon;
  store >> rgbmodeicon;
  store >> hsvmodeicon;
  store >> cmymodeicon;
  store >> txtmodeicon;
  store >> wheel;
  store >> rgbaslider[0] >> rgbaslider[1] >> rgbaslider[2] >> rgbaslider[3];
  store >> hsvaslider[0] >> hsvaslider[1] >> hsvaslider[2] >> hsvaslider[3];
  store >> cmyslider[0]  >> cmyslider[1]  >> cmyslider[2]  >> cmyslider[3];
  store >> rgbatext[0] >> rgbatext[1] >> rgbatext[2] >> rgbatext[3];
  store >> hsvatext[0] >> hsvatext[1] >> hsvatext[2] >> hsvatext[3];
  store >> cmytext[0]  >> cmytext[1]  >> cmytext[2]  >> cmytext[3];
  store >> colorwells[0]  >> colorwells[1]  >> colorwells[2]  >> colorwells[3];
  store >> colorwells[4]  >> colorwells[5]  >> colorwells[6]  >> colorwells[7];
  store >> colorwells[8]  >> colorwells[9]  >> colorwells[10] >> colorwells[11];
  store >> colorwells[12] >> colorwells[13] >> colorwells[14] >> colorwells[15];
  store >> colorwells[16] >> colorwells[17] >> colorwells[18] >> colorwells[19];
  store >> colorwells[20] >> colorwells[21] >> colorwells[22] >> colorwells[23];
  store.load(rgba,4);
  store.load(hsva,4);
  }


// Cleanup; icons must be explicitly deleted
FXColorSelector::~FXColorSelector(){
  delete eyedropicon;
  delete dialmodeicon;
  delete rgbmodeicon;
  delete hsvmodeicon;
  delete cmymodeicon;
  delete txtmodeicon;
  wheel=(FXColorRing*)-1L;
  eyedropicon=(FXIcon*)-1L;
  dialmodeicon=(FXIcon*)-1L;
  rgbmodeicon=(FXIcon*)-1L;
  hsvmodeicon=(FXIcon*)-1L;
  cmymodeicon=(FXIcon*)-1L;
  txtmodeicon=(FXIcon*)-1L;
  panels=(FXTabBook*)-1L;
  well=(FXColorWell*)-1L;
  accept=(FXButton*)-1L;
  cancel=(FXButton*)-1L;
  }

}
