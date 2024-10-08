/********************************************************************************
*                                                                               *
*                    F i l e   S e l e c t i o n   D i a l o g                  *
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
#include "FXElement.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXPath.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXStringDictionary.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXApp.h"
#include "FXRecentFiles.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXComposite.h"
#include "FXPacker.h"
#include "FXShell.h"
#include "FXTopWindow.h"
#include "FXDialogBox.h"
#include "FXFileSelector.h"
#include "FXFileDialog.h"



/*
  Notes:
  - Wraps the FXFileSelector file selection mega widget.
*/

using namespace FX;

/*******************************************************************************/

namespace FX {


// File dialog registry section name
const FXchar FXFileDialog::sectionName[]="File Dialog";


// Object implementation
FXIMPLEMENT(FXFileDialog,FXDialogBox,nullptr,0)


// Construct file fialog box
FXFileDialog::FXFileDialog(FXWindow* own,const FXString& name,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDialogBox(own,name,opts|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE|DECOR_CLOSE,x,y,w,h,4,4,4,4,4,4){
  filebox=new FXFileSelector(this,nullptr,0,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  filebox->acceptButton()->setTarget(this);
  filebox->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
  filebox->cancelButton()->setTarget(this);
  filebox->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
  }


// Construct free-floating file dialog box
FXFileDialog::FXFileDialog(FXApp* a,const FXString& name,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXDialogBox(a,name,opts|DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE|DECOR_CLOSE,x,y,w,h,4,4,4,4,4,4){
  filebox=new FXFileSelector(this,nullptr,0,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  filebox->acceptButton()->setTarget(this);
  filebox->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
  filebox->cancelButton()->setTarget(this);
  filebox->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
  }


// Create server-side resources
void FXFileDialog::create(){
  readRegistry();
  FXDialogBox::create();
  }


// Destroy server-side resources
void FXFileDialog::destroy(){
  FXDialogBox::destroy();
  writeRegistry();
  }


// Load settings from registry
void FXFileDialog::readRegistry(){
  setWidth(getApp()->reg().readIntEntry(sectionName,"width",getWidth()));
  setHeight(getApp()->reg().readIntEntry(sectionName,"height",getHeight()));
  setFileBoxStyle(getApp()->reg().readUIntEntry(sectionName,"style",getFileBoxStyle()));
  showHiddenFiles(getApp()->reg().readBoolEntry(sectionName,"showhidden",showHiddenFiles()));
  }


// Save settings to registry
void FXFileDialog::writeRegistry(){
  getApp()->reg().writeIntEntry(sectionName,"width",getWidth());
  getApp()->reg().writeIntEntry(sectionName,"height",getHeight());
  getApp()->reg().writeUIntEntry(sectionName,"style",getFileBoxStyle());
  getApp()->reg().writeBoolEntry(sectionName,"showhidden",showHiddenFiles());
  }


// Set file name
void FXFileDialog::setFilename(const FXString& path){
  filebox->setFilename(path);
  }


// Get filename, if any
FXString FXFileDialog::getFilename() const {
  return filebox->getFilename();
  }


// Return empty-string terminated list of selected file names,
FXString* FXFileDialog::getFilenames() const {
  return filebox->getFilenames();
  }


// Set directory
void FXFileDialog::setDirectory(const FXString& path){
  filebox->setDirectory(path);
  }


// Get directory
FXString FXFileDialog::getDirectory() const {
  return filebox->getDirectory();
  }


// Set pattern
void FXFileDialog::setPattern(const FXString& ptrn){
  filebox->setPattern(ptrn);
  }


// Get pattern
FXString FXFileDialog::getPattern() const {
  return filebox->getPattern();
  }


// Change patterns, each pattern separated by newline
void FXFileDialog::setPatternList(const FXString& patterns){
  filebox->setPatternList(patterns);
  }


// Return list of patterns
FXString FXFileDialog::getPatternList() const {
  return filebox->getPatternList();
  }


// Set current file pattern from the list
void FXFileDialog::setCurrentPattern(FXint patno){
  filebox->setCurrentPattern(patno);
  }


// Return current pattern
FXint FXFileDialog::getCurrentPattern() const {
  return filebox->getCurrentPattern();
  }


// Change pattern text for pattern number
void FXFileDialog::setPatternText(FXint patno,const FXString& text){
  filebox->setPatternText(patno,text);
  }


// Get pattern text for given pattern number
FXString FXFileDialog::getPatternText(FXint patno) const {
  return filebox->getPatternText(patno);
  }


// Return number of patterns
FXint FXFileDialog::getNumPatterns() const {
  return filebox->getNumPatterns();
  }


// Allow pattern entry
void FXFileDialog::allowPatternEntry(FXbool flag){
  filebox->allowPatternEntry(flag);
  }


// Return true if pattern entry is allowed
FXbool FXFileDialog::allowPatternEntry() const {
  return filebox->allowPatternEntry();
  }


// Change space for item
void FXFileDialog::setItemSpace(FXint s){
  filebox->setItemSpace(s);
  }


// Get space for item
FXint FXFileDialog::getItemSpace() const {
  return filebox->getItemSpace();
  }


// Change File List style
void FXFileDialog::setFileBoxStyle(FXuint style){
  filebox->setFileBoxStyle(style);
  }


// Return File List style
FXuint FXFileDialog::getFileBoxStyle() const {
  return filebox->getFileBoxStyle();
  }


// Change file selection mode
void FXFileDialog::setSelectMode(FXuint mode){
  filebox->setSelectMode(mode);
  }


// Return file selection mode
FXuint FXFileDialog::getSelectMode() const {
  return filebox->getSelectMode();
  }


// Change wildcard matching mode
void FXFileDialog::setMatchMode(FXuint mode){
  filebox->setMatchMode(mode);
  }


// Return wildcard matching mode
FXuint FXFileDialog::getMatchMode() const {
  return filebox->getMatchMode();
  }


// Return true if showing hidden files
FXbool FXFileDialog::showHiddenFiles() const {
  return filebox->showHiddenFiles();
  }


// Show or hide hidden files
void FXFileDialog::showHiddenFiles(FXbool flag){
  filebox->showHiddenFiles(flag);
  }


// Return true if image preview on
FXbool FXFileDialog::showImages() const {
  return filebox->showImages();
  }


// Show or hide preview images
void FXFileDialog::showImages(FXbool flag){
  filebox->showImages(flag);
  }


// Return images preview size
FXint FXFileDialog::getImageSize() const {
  return filebox->getImageSize();
  }


// Change images preview size
void FXFileDialog::setImageSize(FXint size){
  filebox->setImageSize(size);
  }


// Show readonly button
void FXFileDialog::showReadOnly(FXbool flag){
  filebox->showReadOnly(flag);
  }


// Return true if readonly is shown
FXbool FXFileDialog::shownReadOnly() const {
  return filebox->shownReadOnly();
  }


// Set initial state of readonly button
void FXFileDialog::setReadOnly(FXbool flag){
  filebox->setReadOnly(flag);
  }


// Get readonly state
FXbool FXFileDialog::getReadOnly() const {
  return filebox->getReadOnly();
  }


// Allow or disallow navigation
void FXFileDialog::allowNavigation(FXbool flag){
  filebox->allowNavigation(flag);
  }


// Is navigation allowed?
FXbool FXFileDialog::allowNavigation() const{
  return filebox->allowNavigation();
  }


// Set draggable files
void FXFileDialog::setDraggableFiles(FXbool flag){
  filebox->setDraggableFiles(flag);
  }


// Are files draggable?
FXbool FXFileDialog::getDraggableFiles() const {
  return filebox->getDraggableFiles();
  }


// Set file time format
void FXFileDialog::setTimeFormat(const FXString& fmt){
  filebox->setTimeFormat(fmt);
  }


// Return file time format
FXString FXFileDialog::getTimeFormat() const {
  return filebox->getTimeFormat();
  }


// Change file associations
void FXFileDialog::setAssociations(FXFileAssociations* assoc,FXbool owned){
  filebox->setAssociations(assoc,owned);
  }


// Return file associations
FXFileAssociations* FXFileDialog::getAssociations() const {
  return filebox->getAssociations();
  }


// Change icon loader
void FXFileDialog::setIconSource(FXIconSource* src){
  filebox->setIconSource(src);
  }


// Return icon loader
FXIconSource* FXFileDialog::getIconSource() const {
  return filebox->getIconSource();
  }


// Save data
void FXFileDialog::save(FXStream& store) const {
  FXDialogBox::save(store);
  store << filebox;
  }


// Load data
void FXFileDialog::load(FXStream& store){
  FXDialogBox::load(store);
  store >> filebox;
  }


// Cleanup
FXFileDialog::~FXFileDialog(){
  destroy();
  filebox=(FXFileSelector*)-1L;
  }


// Open existing filename
FXString FXFileDialog::getOpenFilename(FXWindow* owner,const FXString& caption,const FXString& path,const FXString& patterns,FXint initial){
  FXFileDialog opendialog(owner,caption);
  FXString filename;
  opendialog.setSelectMode(SELECTFILE_EXISTING);
  opendialog.setPatternList(patterns);
  opendialog.setFilename(path);
  opendialog.setCurrentPattern(initial);
  if(opendialog.execute()){
    filename=opendialog.getFilename();
    if(FXStat::isFile(filename)) return filename;
    }
  return FXString::null;
  }


// Save to filename
FXString FXFileDialog::getSaveFilename(FXWindow* owner,const FXString& caption,const FXString& path,const FXString& patterns,FXint initial){
  FXFileDialog savedialog(owner,caption);
  savedialog.setSelectMode(SELECTFILE_ANY);
  savedialog.setPatternList(patterns);
  savedialog.setFilename(path);
  savedialog.setCurrentPattern(initial);
  if(savedialog.execute()){
    return savedialog.getFilename();
    }
  return FXString::null;
  }


// Open multiple existing files
FXString* FXFileDialog::getOpenFilenames(FXWindow* owner,const FXString& caption,const FXString& path,const FXString& patterns,FXint initial){
  FXFileDialog opendialog(owner,caption);
  opendialog.setSelectMode(SELECTFILE_MULTIPLE);
  opendialog.setPatternList(patterns);
  opendialog.setFilename(path);
  opendialog.setCurrentPattern(initial);
  if(opendialog.execute()){
    return opendialog.getFilenames();
    }
  return nullptr;
  }


// Open existing directory name
FXString FXFileDialog::getOpenDirectory(FXWindow* owner,const FXString& caption,const FXString& path){
  FXFileDialog dirdialog(owner,caption);
  FXString dirname;
  dirdialog.setSelectMode(SELECTFILE_DIRECTORY);
  dirdialog.setFilename(path);
  if(dirdialog.execute()){
    dirname=dirdialog.getFilename();
    if(FXStat::isDirectory(dirname)) return dirname;
    }
  return FXString::null;
  }

}

