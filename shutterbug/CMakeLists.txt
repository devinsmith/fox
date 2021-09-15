
set(FOX_LIBRARIES fox)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:fox,INTERFACE_INCLUDE_DIRECTORIES>)

set(TARGET_NAME shutterbug)
set(SOURCES ShutterBug.cpp Snapper.cpp icons.cpp)
set(HEADERS ShutterBug.h Snapper.h icons.h)

set(ICONS line_0.gif line_1.gif line_2.gif
  line_3.gif line_4.gif line_5.gif line_6.gif
  line_7.gif line_8.gif shutterbug.gif tinyshutterbug.gif)

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

