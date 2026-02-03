add_executable(mptui main.cc)

add_subdirectory(fileNode)
add_subdirectory(fileExplorer)
add_subdirectory(track)
add_subdirectory(musicPlayer)
add_subdirectory(trackQueue)
add_subdirectory(stateWrapper)
add_subdirectory(action)
add_subdirectory(audioEngine)

# Fetch FTXUI
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main)

FetchContent_MakeAvailable(ftxui)

target_compile_features(mptui PRIVATE cxx_std_20)

target_link_libraries(
  mptui
  PRIVATE project_warnings
          fileExplorer
          fileNode
          trackQueue
          musicPlayer
          stateWrapper
          audioEngine
          ftxui::screen
          ftxui::dom
          ftxui::component)
