
set(FOX_LIBRARIES fox)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:fox,INTERFACE_INCLUDE_DIRECTORIES>)

check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
if(HAVE_SYS_WAIT_H)
  add_definitions(-DHAVE_SYS_WAIT_H)
endif()

set(TARGET_NAME pathfinder)
set(SOURCES CopyDialog.cpp icons.cpp PathFinder.cpp Preferences.cpp PropertyDialog.cpp)
set(HEADERS CopyDialog.h icons.h PathFinder.h Preferences.h PropertyDialog.h)

set(ICONS addbook.bmp
  bigicons.bmp
  closepanel.gif
  clrbook.gif
  copy.bmp
  config_gif.gif
  copyit.gif
  cut.bmp
  delbook.bmp
  deleteit.bmp
  desktop.bmp
  details.bmp
  dirup.bmp
  enter.gif
  execute.bmp
  file_gif.gif
  fileshown.gif
  filehidden.gif
  foldernew.gif
  foxbig.gif
  foxmini.gif
  goback.bmp
  goforw.bmp
  gotodir.bmp
  home.gif
  hosts.bmp
  iconpath.gif
  linkit.bmp
  location.gif
  maphost.bmp
  miscellaneous_gif.gif
  moveit.gif
  paste.bmp
  pattern_gif.gif
  properties.bmp
  quit_gif.gif
  newfolder.bmp
  renameit.gif
  rotateleft.gif
  rotateright.gif
  run_bmp.bmp
  setbook.gif
  setdir.gif
  sorting.bmp
  smallicons.bmp
  unmaphost.bmp
  warningicon_gif.gif
  work.gif)

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
target_link_libraries(${TARGET_NAME} PUBLIC fox)

