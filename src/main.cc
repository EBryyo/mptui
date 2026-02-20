#include <chrono>
#include <fileNode/fileNode.hh>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <stateWrapper/stateWrapper.hh>
#include <thread>

#define FILES_PATH "~/Music/"

int main() {
  auto root = createTree(FILES_PATH);
  auto screen = ftxui::ScreenInteractive::Fullscreen();

  auto state = std::make_shared<StateWrapper>(root, &screen);

  auto layout = ftxui::Renderer(state, [&] {
    return ftxui::window(ftxui::text("MPTUI") | ftxui::bold, state->Render()) |
           ftxui::flex;
  });

  // auto layout =
  //     ftxui::Renderer(state, [&] { return state->Render() | ftxui::flex; });

  // Ensure the file explorer has initial keyboard focus.
  state->TakeFocus();

  std::atomic<bool> running = true;

  // Background thread: periodically post a custom event so the UI
  // thread can process actions and refresh audio state.
  std::thread tick_loop = std::thread([&] {
    while (running.load()) {
      screen.PostEvent(ftxui::Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  });

  screen.Loop(layout);

  running.store(false);

  tick_loop.join();

  return 0;
}
