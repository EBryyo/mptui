#include "fileExplorer/fileExplorer.hh"
#include "fileNode/fileNode.hh"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

#define FILES_PATH "~/.local/share/mptui/"

int main(int argc, char **argv) {
  auto root = createTree(FILES_PATH);

  ftxui::Component explorer = std::make_shared<FileExplorer>(root);

  auto screen = ftxui::ScreenInteractive::Fullscreen();
  
  int explorer_size = 30;

  ftxui::Component details = ftxui::Renderer([] {
    return ftxui::text("Details Pane") | ftxui::flex | ftxui::center;
  });

  ftxui::Component split = ftxui::ResizableSplitLeft(
      explorer,
      details,
      &explorer_size);

  auto layout = ftxui::Renderer(split, [&] {
    return ftxui::window(
               ftxui::text("File Explorer"), split->Render()) |
           ftxui::flex;
  });

  screen.Loop(layout);

  return 0;
}
