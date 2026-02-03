add_library(fileNode fileNode.cc fileNode.hh)

# FileNode's public header uses ftxui::Elements, so consumers need
# FTXUI's include directories; link ftxui::dom as PUBLIC.
target_link_libraries(fileNode PUBLIC ftxui::dom PRIVATE track project_warnings)

target_include_directories(fileNode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                           ${CMAKE_SOURCE_DIR}/src)
