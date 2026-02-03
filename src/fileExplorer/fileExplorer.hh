#pragma once

#include <action/action.hh>
#include <fileNode/fileNode.hh>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <queue>

class FileExplorer : public ftxui::ComponentBase {
public:
  FileExplorer(std::shared_ptr<FileNode> root,
               std::queue<std::unique_ptr<Action>> *actions)
      : root_(root), actions_{actions} {
    BuildVisible(*root_);

    render_begin_ = 0;
    render_end_ = visible_nodes_.size();

    hovered_ = -1;
    selected_ = nullptr;
  }

  // Component functions

  bool OnEvent(ftxui::Event event) override;

  ftxui::Element OnRender() override;

  bool Focusable() const override { return true; }

private:
  std::shared_ptr<FileNode> root_;
  std::vector<FileNode *> visible_nodes_;

  int render_begin_;
  int render_end_;

  int hovered_;
  FileNode *selected_;

  int BoxHeight() { return box_.y_max - box_.y_min; }

  ftxui::Box box_;
  bool focused = false;

  std::queue<std::unique_ptr<Action>> *actions_;

  bool handleMouse(ftxui::Event event);
  bool handleKeyboard(ftxui::Event event);
  void onTrackSelect();
  void onDirectorySelect();

  //   void handleSelectTrack(std::shared_ptr<FileNode> node);

  void BuildVisible(FileNode &node);
};
