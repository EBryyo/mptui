#include "fileNode.hh"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

ftxui::Elements renderNode(FileNode &node, FileNode* selected_node, int depth) {
  ftxui::Elements elements;

  std::string prefix = std::string(depth * 2, ' ');
  auto highlight = &node == selected_node ? ftxui::inverted : ftxui::nothing;

  if (node.is_dir) {
    elements.push_back(
        ftxui::text(prefix + (node.expanded ? "▾ ": "▸ ") + node.name) | highlight);

    if (node.expanded) {
      for (auto &child : node.children) {
        ftxui::Elements child_elements = renderNode(*child, selected_node, depth + 1);
        elements.insert(elements.end(), child_elements.begin(),
                        child_elements.end());
      }
    }
  } else {
    elements.push_back(ftxui::text(prefix + node.name) | highlight);
  }

  return elements;
}
void printTree(FileNode &node, int depth) {
  std::string prefix = std::string(depth * 2, ' ');
  std::cout << prefix << node.name << std::endl;
  for (auto& child : node.children) {
    printTree(*child, depth + 1);
  }
}

static void createTreeHelper(std::shared_ptr<FileNode> node) {
  auto fs_iterator = std::filesystem::directory_iterator(node->path);

  for (auto &entry : fs_iterator) {
    auto child_ptr = std::shared_ptr<FileNode>(new FileNode);
    child_ptr->is_root = false;
    child_ptr->is_dir = entry.is_directory();
    child_ptr->expanded = true;
    child_ptr->path = entry.path();
    child_ptr->name = child_ptr->path.filename();
    child_ptr->is_album = false;

    if (child_ptr->is_dir) {
      createTreeHelper(child_ptr);
      child_ptr->is_album =
          std::none_of(child_ptr->children.begin(), child_ptr->children.end(),
                       [](auto child) { return child->is_dir; });
    }

    node->children.push_back(child_ptr);
  }
}

std::shared_ptr<FileNode> createTree(std::string path) {
  auto root = std::shared_ptr<FileNode>(new FileNode);
  auto actual_path = std::filesystem::path(path);
  if (path[0] == '~') {
    if (const char *home = std::getenv("HOME")) {
      auto homestring = std::string(home);
      actual_path = std::filesystem::path(homestring + path.substr(1));
    }
  }
  auto root_path = std::filesystem::path(actual_path);
  root->is_dir = true;
  root->is_root = true;
  root->expanded = true;
  root->path = root_path;

  createTreeHelper(root);
  return root;
}
