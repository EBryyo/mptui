add_library(fileExplorer
    fileExplorer.cc
    fileExplorer.hh
)

target_link_libraries(fileExplorer PRIVATE 
    fileNode
    ftxui::dom
    ftxui::component
    project_warnings
)

target_include_directories(fileNode PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
