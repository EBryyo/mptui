#pragma once

#include <fileNode/fileNode.hh>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

class FileExplorer : public ftxui::ComponentBase {
public:
  FileExplorer(std::shared_ptr<FileNode> root) : root_(root) {
    BuildVisible(*root_);
    if (visible_nodes_.size() > 1) {
      hovered_index_ = 1;
    }
  }

  // Component functions

  bool OnEvent(ftxui::Event event) override;

  ftxui::Element OnRender() override;

  bool Focusable() const override { return true; }

private:
  std::shared_ptr<FileNode> root_;
  std::vector<FileNode *> visible_nodes_;
  FileNode *selected_ = nullptr; // currently selected node
  FileNode *hovered_ = nullptr;  // node being hovered above
  int hovered_index_ = -1;       // -1 means no selection

  ftxui::Box box_;
  bool mouse_hovered_ = false;

  bool handleMouse(ftxui::Event event);
  bool handleKeyboard(ftxui::Event event);

  //   void handleSelectTrack(std::shared_ptr<FileNode> node);

  void BuildVisible(FileNode &node);
};
