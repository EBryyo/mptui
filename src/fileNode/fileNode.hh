#pragma once
#include <filesystem>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <track/track.hh>
#include <vector>

struct FileNode {
  std::string name;
  std::filesystem::path path;
  bool is_dir;
  bool is_album;
  bool is_root;
  bool expanded;
  std::vector<std::shared_ptr<FileNode>> children;
  std::shared_ptr<FileNode> parent;
  std::shared_ptr<Track> track;
};

ftxui::Elements renderNode(FileNode &node, FileNode *hovered_node,
                           FileNode *selected_node, int depth = 0);

std::shared_ptr<FileNode> createTree(std::string path);

void printTree(FileNode &node, int depth = 0);
