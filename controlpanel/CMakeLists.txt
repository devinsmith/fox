
set(FOX_LIBRARIES FOX)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:FOX,INTERFACE_INCLUDE_DIRECTORIES>)

set(TARGET_NAME ControlPanel)
set(SOURCES ControlPanel.cpp icons.cpp)
set(HEADERS ControlPanel.h icons.h)

set(ICONS controlpanel_gif.gif
  colors_gif.gif
  filebinding_gif.gif
  settings_gif.gif)

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

