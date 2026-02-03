add_library(audioEngine STATIC audioEngine.cc miniaudio_impl.cc)

target_link_libraries(audioEngine PRIVATE project_warnings)

target_link_libraries(audioEngine PUBLIC pthread dl m)

target_include_directories(
  audioEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/external)
