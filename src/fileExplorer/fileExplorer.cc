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

bool FileExplorer::handleKeyboard(ftxui::Event event) {
  const int total = (int)visible_nodes_.size();
  if (total == 0) {
    return false;
  }

  const int box_height = BoxHeight();

  // Activate the hovered item.
  if (event == ftxui::Event::Return) {
    if (hovered_ < 0 || hovered_ >= total) {
      return false;
    }

    if (visible_nodes_[hovered_]->is_dir) {
      onDirectorySelect();
    } else {
      onTrackSelect();
    }
    return true;
  }

  // Clear hover.
  if (event == ftxui::Event::Escape) {
    if (hovered_ == -1) {
      return false;
    }
    hovered_ = -1;
    return true;
  }

  // Move hover down.
  if (event == ftxui::Event::ArrowDown) {
    if (hovered_ == -1) {
      hovered_ = std::max(0, render_begin_);
    } else if (hovered_ < total - 1) {
      ++hovered_;
    } else {
      return false; // already at last item
    }

    if (box_height > 0 && total > box_height) {
      if (render_begin_ < 0) {
        render_begin_ = 0;
      }
      if (render_end_ <= render_begin_) {
        render_end_ = render_begin_ + box_height;
      }

      if (hovered_ >= render_end_) {
        render_end_ = hovered_ + 1;
        render_begin_ = render_end_ - box_height;
      }

      if (render_begin_ < 0) {
        render_begin_ = 0;
      }
      if (render_end_ > total) {
        render_end_ = total;
        render_begin_ = std::max(0, render_end_ - box_height);
      }
    }
    return true;
  }

  // Move hover up.
  if (event == ftxui::Event::ArrowUp) {
    if (hovered_ == -1) {
      if (box_height > 0 && total > box_height) {
        hovered_ = std::min(total - 1, render_begin_ + box_height - 1);
      } else {
        hovered_ = total - 1;
      }
    } else if (hovered_ > 0) {
      --hovered_;
    } else {
      return false; // already at first item
    }

    if (box_height > 0 && total > box_height) {
      if (render_begin_ < 0) {
        render_begin_ = 0;
      }
      if (render_end_ <= render_begin_) {
        render_end_ = render_begin_ + box_height;
      }

      if (hovered_ < render_begin_) {
        render_begin_ = hovered_;
        render_end_ = render_begin_ + box_height;
      }

      if (render_begin_ < 0) {
        render_begin_ = 0;
        render_end_ = std::min(total, box_height);
      }
      if (render_end_ > total) {
        render_end_ = total;
        render_begin_ = std::max(0, render_end_ - box_height);
      }
    }
    return true;
  }

  return false;
}

bool FileExplorer::OnEvent(ftxui::Event event) {
  bool flag = event.is_mouse() ? handleMouse(event) : handleKeyboard(event);
  if (flag) {
    visible_nodes_.clear();
    BuildVisible(*root_);
  }
  return flag;
}

namespace {
FileNode *FindNodeForTrack(FileNode *node,
                           const std::shared_ptr<Track> &track) {
  if (node->track == track) {
    return node;
  }

  for (auto &child : node->children) {
    if (auto *found = FindNodeForTrack(child.get(), track)) {
      return found;
    }
  }
  return nullptr;
}
} // namespace

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

void FileExplorer::SelectTrack(std::shared_ptr<Track> track) {
  if (!track) {
    return;
  }

  auto *node = FindNodeForTrack(root_.get(), track);
  if (!node) {
    return;
  }

  selected_ = node;

  // Ensure all parents are expanded so the selected node is visible.
  auto parent = node->parent;
  while (parent) {
    parent->expanded = true;
    parent = parent->parent;
  }

  // Rebuild the visible node list to reflect any expansion changes.
  visible_nodes_.clear();
  BuildVisible(*root_);

  // Position hover and scroll window so the selected node is visible.
  int selected_index = -1;
  for (int i = 0; i < (int)visible_nodes_.size(); ++i) {
    if (visible_nodes_[i] == node) {
      selected_index = i;
      break;
    }
  }

  if (selected_index == -1) {
    return;
  }

  hovered_ = selected_index;

  const int box_height = BoxHeight();
  if (box_height <= 0) {
    render_begin_ = 0;
    render_end_ = (int)visible_nodes_.size();
    return;
  }

  if (selected_index < render_begin_ || selected_index >= render_end_) {
    render_begin_ = std::max(0, selected_index - box_height / 2);
    render_end_ = std::min((int)visible_nodes_.size(), render_begin_ + box_height);
  }
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

    // If the selected track belongs to an album directory, build a
    // queue containing the entire album and attach it to the action.
    auto parent = selected_->parent;
    if (parent && parent->is_album) {
      std::vector<std::shared_ptr<Track>> album_tracks;
      album_tracks.reserve(parent->children.size());
      for (auto &child : parent->children) {
        if (child->track) {
          album_tracks.push_back(child->track);
        }
      }

      if (!album_tracks.empty()) {
        action->queue = album_tracks;
      }
    }
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
