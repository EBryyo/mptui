#include <fileNode/fileNode.hh>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <stateWrapper/stateWrapper.hh>

#define FILES_PATH "~/Music/"

int main(int argc, char **argv) {
  auto root = createTree(FILES_PATH);

  ftxui::Component state = std::make_shared<StateWrapper>(root);

  auto screen = ftxui::ScreenInteractive::Fullscreen();

  auto layout = ftxui::Renderer(state, [&] {
    return ftxui::window(ftxui::text("MPTUI") | ftxui::bold, state->Render()) |
           ftxui::flex;
  });

  // auto layout =
  //     ftxui::Renderer(state, [&] { return state->Render() | ftxui::flex; });

  // Ensure the file explorer has initial keyboard focus.
  state->TakeFocus();

  screen.Loop(layout);

  return 0;
}
