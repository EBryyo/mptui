#include <algorithm>
#include <fileNode.hh>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

ftxui::Elements renderNode(FileNode &node, FileNode *hovered_node,
                           FileNode *selected_node, int depth) {
  ftxui::Elements elements;

  std::string prefix = std::string(depth * 2, ' ');
  auto highlight = &node == selected_node ? ftxui::inverted
                   : &node == hovered_node
                       ? ftxui::bgcolor(ftxui::Color::DarkBlue)
                       : ftxui::nothing;

  if (node.is_dir) {
    if (!node.is_root) {
      elements.push_back(
          ftxui::text(prefix + (node.expanded ? "▾ " : "▸ ") + node.name) |
          highlight | ftxui::bold);
    }

    if (node.expanded) {
      for (auto &child : node.children) {
        ftxui::Elements child_elements =
            renderNode(*child, hovered_node, selected_node, depth + 1);
        elements.insert(elements.end(), child_elements.begin(),
                        child_elements.end());
      }
    }
  } else {
    elements.push_back(ftxui::text(prefix + "- " + node.track->title) |
                       highlight);
  }
  return elements;
}

void printTree(FileNode &node, int depth) {
  std::string prefix = std::string(depth * 2, ' ');
  std::cout << prefix << node.name << std::endl;
  for (auto &child : node.children) {
    printTree(*child, depth + 1);
  }
}

static void createTreeHelper(std::shared_ptr<FileNode> node) {
  auto fs_iterator = std::filesystem::directory_iterator(node->path);

  for (const auto &entry : fs_iterator) {
    auto child_ptr = std::shared_ptr<FileNode>(new FileNode);
    child_ptr->is_root = false;
    child_ptr->is_dir = entry.is_directory();
    child_ptr->expanded = true;
    child_ptr->path = entry.path();
    child_ptr->name = child_ptr->path.stem();
    child_ptr->parent = node;

    if (child_ptr->is_dir) {
      createTreeHelper(child_ptr);
      child_ptr->is_album =
          std::none_of(child_ptr->children.begin(), child_ptr->children.end(),
                       [](auto child) { return child->is_dir; });
      if (child_ptr->is_album) {
        child_ptr->expanded = !child_ptr->is_album;
      }
    } else {
      child_ptr->track = readMetadata(child_ptr->path);
    }

    if (child_ptr->is_dir || child_ptr->path.extension() == ".mp3" ||
        child_ptr->path.extension() == ".flac") {
      node->children.push_back(child_ptr);
    }
  }
  std::sort(node->children.begin(), node->children.end(),
            [](std::shared_ptr<FileNode> &a, std::shared_ptr<FileNode> &b) {
              if (a->track && b->track && a->track->index != -1 &&
                  b->track->index != -1) {
                return a->track->index < b->track->index;
              } else {
                return a->name.compare(b->name) < 0;
              }
            });
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
