add_library(trackQueue trackQueue.cc trackQueue.hh)

target_link_libraries(trackQueue PRIVATE project_warnings ftxui::dom
                                         ftxui::component)

target_include_directories(trackQueue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                             ${CMAKE_SOURCE_DIR}/src)
