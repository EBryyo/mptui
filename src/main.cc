#include "fileExplorer/fileExplorer.hh"
#include "fileNode/fileNode.hh"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

#define FILES_PATH "~/.local/share/mptui/"

int main(int argc, char **argv) {
  auto root = createTree(FILES_PATH);

  ftxui::Component explorer = std::make_shared<FileExplorer>(root);

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  screen.Loop(explorer);

  return 0;
}
