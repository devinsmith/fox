
set(FOX_LIBRARIES FOX)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:FOX,INTERFACE_INCLUDE_DIRECTORIES>)

check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
if(HAVE_SYS_WAIT_H)
  add_definitions(-DHAVE_SYS_WAIT_H)
endif()

set(TARGET_NAME pathfinder)
set(SOURCES CopyDialog.cpp icons.cpp PathFinder.cpp Preferences.cpp PropertyDialog.cpp)
set(HEADERS CopyDialog.h icons.h PathFinder.h Preferences.h PropertyDialog.h)

set(ICONS
  bigicons.bmp
  blocks_gif.gif
  bookdel_gif.gif
  booknxt_gif.gif
  bookprv_gif.gif
  bookclr_gif.gif
  bookset_gif.gif
  bookmrk_gif.gif
  closepanel.gif
  copy_gif.gif
  config_gif.gif
  cut_gif.gif
  delete_gif.gif
  desktop.bmp
  details.bmp
  dirup_gif.gif
  enter.gif
  file_gif.gif
  fileshown.gif
  filehidden.gif
  foldernew_gif.gif
  foxbig.gif
  foxmini.gif
  goup_gif.gif
  godown_gif.gif
  goback_gif.gif
  goforw_gif.gif
  gotodir.bmp
  home_gif.gif
  hosts.bmp
  iconpath.gif
  link_gif.gif
  location.gif
  maphost.bmp
  miscellaneous_gif.gif
  move_gif.gif
  paste.gif
  pattern_gif.gif
  property_gif.gif
  quit_gif.gif
  newfolder.bmp
  rename_gif.gif
  rotateleft.gif
  rotateright.gif
  run_bmp.bmp
  search.gif
  setdir.gif
  sorting.bmp
  smallicons.bmp
  unmaphost.bmp
  warningicon_gif.gif
  work_gif.gif)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${TARGET_NAME} PRIVATE ${FOX_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC FOX)

