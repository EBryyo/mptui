#pragma once

#include "../fileNode/fileNode.hh"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

class FileExplorer : public ftxui::ComponentBase {
public:
  FileExplorer(std::shared_ptr<FileNode> root) : root_(root) {}

  bool OnEvent(ftxui::Event event) override;
  ftxui::Element OnRender() override;

private:
  std::shared_ptr<FileNode> root_;
  FileNode *selected_ = nullptr;
};
