#include "fileExplorer.hh"
#include <ftxui/dom/elements.hpp>

bool FileExplorer::OnEvent(ftxui::Event event) { return true; }

ftxui::Element FileExplorer::OnRender() { 
    //return ftxui::text("I am rendering") | ftxui::border | ftxui::flex;
    return ftxui::vbox(renderNode(*root_)) | ftxui::border | ftxui::flex; 
}
