add_library(action
    fileNode.cc
    fileNode.hh
)

target_link_libraries(action PRIVATE
    track
    project_warnings
)

target_include_directories(action PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)
