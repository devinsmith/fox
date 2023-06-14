
set(FOX_LIBRARIES FOX)
set(FOX_INCLUDE_DIRS $<TARGET_PROPERTY:FOX,INTERFACE_INCLUDE_DIRECTORIES>)

set(TARGET_NAME adie)
set(SOURCES Adie.cpp Commands.cpp FindInFiles.cpp help.cpp HelpWindow.cpp
  icons.cpp main.cpp Modeline.cpp Preferences.cpp ShellCommand.cpp ShellDialog.cpp
  Syntax.cpp SyntaxParser.cpp TextWindow.cpp)
set(HEADERS Adie.h Commands.h FindInFiles.h help.h HelpWindow.h icons.h
  Modeline.h Preferences.h ShellCommand.h ShellDialog.h Syntax.h SyntaxParser.h
  TextWindow.h)

set(ICONS adie_gif.gif
  backward_gif.gif
  big_gif.gif
  bookclr2_gif.gif
  bookdel2_gif.gif
  bookmrk2_gif.gif
  booknxt2_gif.gif
  bookprv2_gif.gif
  bookset2_gif.gif
  browser.gif
  capitalize.gif
  close_gif.gif
  colors_gif.gif
  command_bmp.bmp
  config_gif.gif
  copy_gif.gif
  cut_gif.gif
  delete_gif.gif
  delimit_gif.gif
  docs_gif.gif
  export_gif.gif
  express_gif.gif
  filehidden_gif.gif
  fileshown_gif.gif
  filter_gif.gif
  fonts_gif.gif
  forward_gif.gif
  goto_gif.gif
  help_gif.gif
  import_gif.gif

  indent_gif.gif
  info_gif.gif
  lang_gif.gif
  logger.gif
  lowercase.gif
  miscellaneous_gif.gif
  new_gif.gif
  newfile_gif.gif
  nobrowser.gif
  nologger.gif
  nowrap_gif.gif
  open_gif.gif
  opensel_gif.gif
  palette_gif.gif
  pattern_gif.gif
  paste_gif.gif
  point_gif.gif
  point2_gif.gif
  print_gif.gif
  quit_gif.gif
  redo_gif.gif
  reload_gif.gif
  replace_gif.gif
  save_gif.gif
  saveall_gif.gif
  saveas_gif.gif
  saveto_gif.gif
  script_gif.gif
  search_gif.gif
  search_case.gif
  search_dn.gif
  search_nocase.gif
  search_norex.gif
  search_noword.gif
  search_rex.gif
  search_up.gif
  search_word.gif
  searchfiles_gif.gif
  searchicon_gif.gif
  searchnext_gif.gif
  searchprev_gif.gif
  shiftleft_gif.gif
  shiftright_gif.gif
  small_gif.gif
  styles_gif.gif
  syntax_gif.gif
  switch_gif.gif
  terminal_gif.gif
  undo_gif.gif
  uppercase.gif
  wordwrap_gif.gif
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/help.cpp ${CMAKE_CURRENT_SOURCE_DIR}/help.h
  COMMAND reswrap -h -a -o help.h help.txt
  COMMAND reswrap -i help.h -a -o help.cpp help.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${TARGET_NAME} PRIVATE ${FOX_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC FOX)

