#include <algorithm>
#include <fileExplorer.hh>
#include <ftxui/dom/elements.hpp>

bool FileExplorer::handleMouse(ftxui::Event event) {
  bool flag = false;
  // Handle mouse events if necessary
  auto &m = event.mouse();

  if (!box_.Contain(m.x, m.y)) {
    mouse_hovered_ = false;
    if (hovered_index_ != -1) {
      hovered_index_ = -1;
      return true;
    }
    return false;
  }

  mouse_hovered_ = true;

  if (m.y > 0 && m.y < (int)visible_nodes_.size()) {
    hovered_index_ = m.y;
    FileNode *node = visible_nodes_[hovered_index_];
    if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed) {
      if (node->is_dir) {
        node->expanded = !node->expanded;
      } else {
        // Handle file selection here
        selected_ = node;
      }
      flag = true;
    }
  } else {
    if (hovered_index_ != -1)
      flag = true;
    hovered_index_ = -1; // No selection
  }
  return flag;
}

bool FileExplorer::handleKeyboard(ftxui::Event event) {
  bool flag = false;

  if (event == ftxui::Event::ArrowDown) {
    hovered_index_ =
        std::min(hovered_index_ + 1, (int)visible_nodes_.size() - 1);
    flag = true;
  }

  else if (event == ftxui::Event::ArrowUp) {
    hovered_index_ = std::max(hovered_index_ - 1, 1);
    flag = true;
  }

  else if (event == ftxui::Event::Return) {
    FileNode *node = visible_nodes_[hovered_index_];
    if (node->is_dir) {
      node->expanded = !node->expanded;
    } else {
      // Handle file selection here
      selected_ = node;
    }
    flag = true;
  }

  else if (event == ftxui::Event::Escape) {
    // Handle exit or other action
    hovered_index_ = -1;
    flag = true;
  }

  return flag;
}

bool FileExplorer::OnEvent(ftxui::Event event) {
  bool flag = false;

  if (event.is_mouse()) {
    flag = handleMouse(event);
  } else {
    flag = handleKeyboard(event);
  }

  if (flag) {
    visible_nodes_.clear();
    BuildVisible(*root_);
  }

  return flag;
}

ftxui::Element FileExplorer::OnRender() {
  // return ftxui::text("I am rendering") | ftxui::border | ftxui::flex;
  return ftxui::vbox(renderNode(
             *root_,
             hovered_index_ == -1 ? nullptr : visible_nodes_[hovered_index_],
             selected_)) |
         ftxui::flex | ftxui::reflect(box_);
}

void FileExplorer::BuildVisible(FileNode &node) {
  visible_nodes_.push_back(&node);
  if (node.is_dir && node.expanded) {
    for (auto &child : node.children) {
      BuildVisible(*child);
    }
  }
}
