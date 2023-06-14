
set(FOX_LIBRARIES FOX)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:FOX,INTERFACE_INCLUDE_DIRECTORIES>)

SET(CHARTLIB_SOURCES FX2DChart.cpp
  FX2DPlot.cpp FXAxis.cpp FXChart.cpp FXCurve.cpp chartutils.h chartutils.cpp)

set(ICONS marble.bmp)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

# Define library
add_library(chart STATIC ${CHARTLIB_SOURCES})
target_include_directories(chart PRIVATE ${FOX_INCLUDE_DIRS})

set(TARGET_NAME charttest)
set(SOURCES charttest.cpp icons.cpp)
set(HEADERS icons.h)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${TARGET_NAME} PRIVATE ${FOX_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC chart FOX)

