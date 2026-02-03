#include <action/action.hh>
#include <algorithm>
#include <fileExplorer.hh>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <track/track.hh>

bool FileExplorer::handleMouse(ftxui::Event event) {
  const auto m = event.mouse();

  if (!box_.Contain(m.x, m.y)) {
    if (hovered_ > -1) {
      hovered_ = -1;
      return true;
    }
    return false;
  }

  const auto index = m.y - box_.y_min;

  if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed &&
      hovered_ > -1) {
    visible_nodes_[hovered_]->is_dir ? onDirectorySelect() : onTrackSelect();
    return true;
  } else if (m.button == ftxui::Mouse::WheelDown) {
    const int view_height = BoxHeight();
    const int total = (int)visible_nodes_.size();

    if (view_height <= 0 || total <= view_height) {
      return false; // nothing to scroll
    }

    // already at the bottom
    if (render_end_ >= total) {
      return false;
    }

    ++render_begin_;
    ++render_end_;
    return true;
  } else if (m.button == ftxui::Mouse::WheelUp) {
    const int view_height = BoxHeight();
    const int total = (int)visible_nodes_.size();

    if (view_height <= 0 || total <= view_height) {
      return false; // nothing to scroll
    }

    // already at the top
    if (render_begin_ <= 0) {
      return false;
    }

    --render_begin_;
    --render_end_;
    return true;
  } else {
    const int visible_count =
        std::min(render_end_ - render_begin_,
                 (int)visible_nodes_.size() - render_begin_);

    if (index >= 0 && index < visible_count) {
      hovered_ = index + render_begin_;
    } else {
      hovered_ = -1;
    }
    return true;
  }

  return false;
}

bool FileExplorer::handleKeyboard(ftxui::Event event) { return false; }

bool FileExplorer::OnEvent(ftxui::Event event) {
  bool flag = event.is_mouse() ? handleMouse(event) : handleKeyboard(event);
  if (flag) {
    visible_nodes_.clear();
    BuildVisible(*root_);
  }
  return flag;
}

ftxui::Element FileExplorer::OnRender() {
  auto nodes = renderNode(
      *root_, hovered_ > -1 ? visible_nodes_[hovered_] : nullptr, selected_);

  const int total = (int)nodes.size();
  const int box_height = BoxHeight();

  if (box_height > 0 && total > box_height) {
    // Clamp render_begin_ into a valid range based on the current box height.
    if (render_begin_ < 0) {
      render_begin_ = 0;
    }
    if (render_begin_ > total - box_height) {
      render_begin_ = total - box_height;
    }

    render_end_ = std::min(total, render_begin_ + box_height);

    ftxui::Elements visible;
    visible.reserve(render_end_ - render_begin_);
    for (int i = render_begin_; i < render_end_; ++i) {
      visible.push_back(nodes[i]);
    }
    nodes = std::move(visible);
  } else {
    // Everything fits; reset window to cover all nodes.
    render_begin_ = 0;
    render_end_ = total;
  }

  return ftxui::vbox(nodes) | ftxui::flex | ftxui::reflect(box_);
}

void FileExplorer::BuildVisible(FileNode &node) {
  if (!node.is_root)
    visible_nodes_.push_back(&node);
  if (node.is_dir && node.expanded) {
    for (auto &child : node.children) {
      BuildVisible(*child);
    }
  }
}

void FileExplorer::onTrackSelect() {
  if (hovered_ == -1)
    return;

  std::unique_ptr<Action> action = std::unique_ptr<Action>(new Action());

  if (selected_ == visible_nodes_[hovered_]) {
    action->type = PAUSE_RESUME;
  } else {
    selected_ = visible_nodes_[hovered_];
    action->type = SELECT;
    action->track = selected_->track;
  }

  actions_->push(std::move(action));
}

namespace {
int get_dir_expanded_size(FileNode *node) {
  int result = node->children.size();
  for (auto &child : node->children) {
    if (child->is_dir) {
      result += get_dir_expanded_size(child.get());
    }
  }
  return result;
}
}; // namespace

void FileExplorer::onDirectorySelect() {
  auto dir = visible_nodes_[hovered_];
  const auto dir_size = get_dir_expanded_size(dir);

  if (dir->expanded) {
    dir->expanded = false;
  } else {
    dir->expanded = true;
  }
}
