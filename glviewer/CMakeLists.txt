
set(FOX_LIBRARIES fox)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:fox,INTERFACE_INCLUDE_DIRECTORIES>)

set(TARGET_NAME glviewer)
set(SOURCES FXGLCone.cpp FXGLCube.cpp FXGLCylinder.cpp FXGLLine.cpp
  FXGLPoint.cpp FXGLShape.cpp FXGLSphere.cpp FXGLTriangleMesh.cpp glviewer.cpp)
set(HEADERS FXGLCone.h FXGLCube.h FXGLCylinder.h FXGLLine.h FXGLPoint.h FXGLShape.h
  FXGLSphere.h FXGLTriangleMesh.h)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${TARGET_NAME} PRIVATE ${FOX_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC fox)

