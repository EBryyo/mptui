add_library(track track.hh track.cc)

find_package(TagLib REQUIRED)

target_link_libraries(track PRIVATE TagLib::tag fileNode project_warnings)

target_include_directories(track PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                        ${CMAKE_SOURCE_DIR}/src)
