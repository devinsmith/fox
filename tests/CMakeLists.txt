
set(FOX_LIBRARIES FOX)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:FOX,INTERFACE_INCLUDE_DIRECTORIES>)

# Don't build gltest for now.
# Don't build math for now (broken under MSVC?)
set(FOX_TESTS bitmapviewer button calendar codecs console datatarget dctest
  dialog dirlist expression format foursplit gaugetest
  groupbox half header hello2 hello iconlist image imageviewer layout
  match mditest memmap minheritance parallel process ratio rex
  scan scribble shutter splitter switcher tabbook table thread timefmt
  unicode variant wizard xml)

foreach (FOX_TEST IN LISTS FOX_TESTS)
  add_executable(${FOX_TEST} "${FOX_TEST}.cpp")
  target_include_directories(${FOX_TEST} PRIVATE ${FOX_INCLUDE_DIRS})
  target_link_libraries(${FOX_TEST} PUBLIC FOX)
endforeach()

