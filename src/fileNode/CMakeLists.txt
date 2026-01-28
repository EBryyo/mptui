add_library(fileNode
    fileNode.cc
    fileNode.hh
)

target_link_libraries(fileNode PRIVATE
    project_warnings
    ftxui::dom
)

target_include_directories(fileNode PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
