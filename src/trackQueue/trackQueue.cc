#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <trackQueue.hh>

ftxui::Element TrackQueue::OnRender() {
  ftxui::Elements tracks;
  tracks.push_back(ftxui::text("Track list"));

  const int total = (int)track_queue_.size();

  // Compute how many rows (excluding the header) we can show.
  const int box_height = BoxHeight();
  const int view_height = box_height > 0 ? std::max(0, box_height - 1) : total;

  if (view_height > 0 && total > view_height) {
    if (render_begin_ < 0)
      render_begin_ = 0;
    if (render_begin_ > total - view_height)
      render_begin_ = total - view_height;

    render_end_ = std::min(total, render_begin_ + view_height);
  } else {
    render_begin_ = 0;
    render_end_ = total;
  }

  for (int i = render_begin_; i < render_end_; ++i) {
    auto &track = track_queue_[i];

    auto highlight = i == index_     ? ftxui::inverted
                     : i == hovered_ ? ftxui::bgcolor(ftxui::Color::DarkBlue)
                                     : ftxui::nothing;

    tracks.push_back(ftxui::text(std::to_string(track->index) + " " +
                                 track->title) |
                     highlight);
  }

  return ftxui::vbox(tracks) | ftxui::reflect(box_);
}

bool TrackQueue::handleMouse(ftxui::Event event) {
  const auto m = event.mouse();

  const int total = (int)track_queue_.size();
  const int box_height = BoxHeight();
  const int view_height = box_height > 0 ? std::max(0, box_height - 1) : total;

  if (!box_.Contain(m.x, m.y)) {
    // Mouse left the track list region: clear hover state.
    if (hovered_ != -1) {
      hovered_ = -1;
      return true;
    }
    return false;
  }

  // Row 0 is the "Track list" header; items start at row 1.
  const int index_in_view = m.y - box_.y_min - 1;

  const int visible_count = std::max(0, render_end_ - render_begin_);

  if (index_in_view < 0 || index_in_view >= visible_count) {
    if (hovered_ != -1) {
      hovered_ = -1;
      return true;
    }
    return false;
  }

  hovered_ = render_begin_ + index_in_view;

  if (m.button == ftxui::Mouse::WheelDown) {
    if (view_height > 0 && total > view_height && render_end_ < total) {
      ++render_begin_;
      ++render_end_;
      return true;
    }
    return false;
  }

  if (m.button == ftxui::Mouse::WheelUp) {
    if (view_height > 0 && total > view_height && render_begin_ > 0) {
      --render_begin_;
      --render_end_;
      return true;
    }
    return false;
  }

  if (m.button == ftxui::Mouse::Left &&
      m.motion == ftxui::Mouse::Pressed) {
    if (track_queue_.empty()) {
      return false;
    }

    auto action = std::make_unique<Action>();

    if (index_ == hovered_) {
      action->type = PAUSE_RESUME;
    } else {
      index_ = hovered_;
      action->type = SELECT;
      action->track = track_queue_[index_];
    }

    actions_->push(std::move(action));
    return true;
  }

  return true; // hover updated
}

bool TrackQueue::handleKeyboard(ftxui::Event event) {
  const int total = (int)track_queue_.size();
  if (total == 0) {
    return false;
  }

  if (event == ftxui::Event::Return) {
    if (hovered_ < 0 || hovered_ >= total) {
      return false;
    }

    auto action = std::make_unique<Action>();

    if (index_ == hovered_) {
      action->type = PAUSE_RESUME;
    } else {
      index_ = hovered_;
      action->type = SELECT;
      action->track = track_queue_[index_];
    }

    actions_->push(std::move(action));
    return true;
  }

  if (event == ftxui::Event::Escape) {
    if (hovered_ == -1) {
      return false;
    }
    hovered_ = -1;
    return true;
  }

  if (event == ftxui::Event::ArrowDown) {
    if (hovered_ == -1) {
      hovered_ = index_ != -1 ? index_ : 0;
    } else if (hovered_ < total - 1) {
      ++hovered_;
    } else {
      return false;
    }

    // Keep hovered item in view when navigating with the keyboard.
    const int box_height = BoxHeight();
    const int view_height = box_height > 0 ? std::max(0, box_height - 1) : total;
    if (view_height > 0 && total > view_height) {
      if (hovered_ >= render_end_) {
        render_end_ = hovered_ + 1;
        render_begin_ = std::max(0, render_end_ - view_height);
      }
    }
    return true;
  }

  if (event == ftxui::Event::ArrowUp) {
    if (hovered_ == -1) {
      hovered_ = index_ != -1 ? index_ : total - 1;
    } else if (hovered_ > 0) {
      --hovered_;
    } else {
      return false;
    }

    // Keep hovered item in view when navigating with the keyboard.
    const int box_height = BoxHeight();
    const int view_height = box_height > 0 ? std::max(0, box_height - 1) : total;
    if (view_height > 0 && total > view_height) {
      if (hovered_ < render_begin_) {
        render_begin_ = hovered_;
        render_end_ = std::min(total, render_begin_ + view_height);
      }
    }
    return true;
  }

  return false;
}

bool TrackQueue::OnEvent(ftxui::Event event) {
  if (!event.is_mouse()) {
    return false;
  }
  return handleMouse(event);
}
