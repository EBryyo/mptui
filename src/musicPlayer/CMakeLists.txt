add_library(musicPlayer musicPlayer.hh musicPlayer.cc stb_image_impl.cc)

target_link_libraries(musicPlayer PRIVATE ftxui::dom ftxui::component
                                          project_warnings)

target_include_directories(musicPlayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
)
