add_library(stateWrapper stateWrapper.cc stateWrapper.hh)

target_link_libraries(
  stateWrapper PRIVATE musicPlayer track fileExplorer project_warnings
                       ftxui::dom ftxui::component)

target_include_directories(stateWrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${CMAKE_SOURCE_DIR}/src)
