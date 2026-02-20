#include <algorithm>
#include <cmath>
#include <musicPlayer/musicPlayer.hh>
#include "stb_image.h"

namespace {
// Render the current track's embedded cover art as a block of
// colored cells sized to fit within a target grid.
// If decoding fails or no art is present, returns an empty element.
ftxui::Element RenderCoverAsBraille(const std::shared_ptr<Track> &track,
                                    int target_cells_w,
                                    int target_cells_h) {
  if (!track || track->image_data.empty()) {
    return ftxui::text("");
  }

  int w = 0;
  int h = 0;
  int channels = 0;
  unsigned char *data = stbi_load_from_memory(
      track->image_data.data(), (int)track->image_data.size(), &w, &h,
      &channels, 3);
  if (!data || w <= 0 || h <= 0) {
    if (data)
      stbi_image_free(data);
    return ftxui::text("");
  }

  if (target_cells_w <= 0)
    target_cells_w = 24;
  if (target_cells_h <= 0)
    target_cells_h = 12;

  int cells_w = std::min(target_cells_w, w);
  int cells_h = std::min(target_cells_h, h);

  float step_x = (float)w / (float)cells_w;
  float step_y = (float)h / (float)cells_h;

  ftxui::Elements rows;
  rows.reserve(cells_h);

  for (int cy = 0; cy < cells_h; ++cy) {
    ftxui::Elements row;
    row.reserve(cells_w);

    int y0 = (int)(cy * step_y);
    int y1 = (int)((cy + 1) * step_y);
    if (y1 <= y0)
      y1 = y0 + 1;
    if (y1 > h)
      y1 = h;

    for (int cx = 0; cx < cells_w; ++cx) {
      int x0 = (int)(cx * step_x);
      int x1 = (int)((cx + 1) * step_x);
      if (x1 <= x0)
        x1 = x0 + 1;
      if (x1 > w)
        x1 = w;

      long sum_r = 0;
      long sum_g = 0;
      long sum_b = 0;
      long count = 0;

      for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
          unsigned char *pixel = data + (y * w + x) * 3;
          sum_r += pixel[0];
          sum_g += pixel[1];
          sum_b += pixel[2];
          ++count;
        }
      }

      unsigned char r = 0;
      unsigned char g = 0;
      unsigned char b = 0;
      if (count > 0) {
        r = (unsigned char)(sum_r / count);
        g = (unsigned char)(sum_g / count);
        b = (unsigned char)(sum_b / count);
      }

      // Use a space with a colored background so the cover art is
      // represented purely as color blocks, without braille glyphs.
      auto cell = ftxui::text(" ") |
                  ftxui::bgcolor(ftxui::Color::RGB(r, g, b));
      row.push_back(cell);
    }

    rows.push_back(ftxui::hbox(std::move(row)));
  }

  stbi_image_free(data);
  return ftxui::vbox(std::move(rows));
}

// Render a simple "bouncy" horizontal bar visualizer, using the
// current playback position as a phase input so it animates over time.
ftxui::Element RenderBouncyBars(int rows, int cols, double t_seconds) {
  if (rows <= 0 || cols <= 0)
    return ftxui::text("");

  ftxui::Elements lines;
  lines.reserve(rows);

  // Speed factor for animation.
  double omega = 4.0;

  for (int i = 0; i < rows; ++i) {
    double phase = i * 0.8;
    double v = std::sin(t_seconds * omega + phase) * 0.5 + 0.5; // 0..1
    int filled = std::max(1, static_cast<int>(v * cols));

    ftxui::Elements cells;
    cells.reserve(cols);

    for (int j = 0; j < filled; ++j) {
      cells.push_back(ftxui::text(" ") |
                      ftxui::bgcolor(ftxui::Color::Green));
    }
    for (int j = filled; j < cols; ++j) {
      cells.push_back(ftxui::text(" "));
    }

    lines.push_back(ftxui::hbox(std::move(cells)));
  }

  return ftxui::vbox(std::move(lines));
}
} // namespace

ftxui::Element MusicPlayer::OnRender() {
  ftxui::Element title;

  // Compute the available area for the whole music player from the
  // previous frame. This drives how large we render the cover art.
  int total_w = box_.x_max - box_.x_min + 1;
  int total_h = box_.y_max - box_.y_min + 1;

  if (track_ && !track_->image_data.empty()) {
    // Prefer showing the embedded cover art when available. Cache the
    // rendered block so we don't re-decode the image every frame.
    // Use the total component box to approximate how much space we can
    // give to the cover art, leaving room for controls, progress, and
    // volume rows.
    if (total_w <= 0)
      total_w = 32;
    if (total_h <= 0)
      total_h = 12;

    // Reserve a few rows for controls/progress/volume and use the rest
    // for the cover. Height adapts to available space; width always
    // stays exactly twice the height (2:1 column:row) but is clamped to
    // fit inside the component width.
    int reserved_rows = 3 /*controls*/ + 1 /*progress*/ + 1 /*volume*/ +
              3 /*spacers*/;

    int max_rows_vertical = std::max(2, total_h - reserved_rows);
    int max_rows_horizontal = std::max(1, total_w / 2);

    int cover_rows = std::min(max_rows_vertical, max_rows_horizontal);
    int cover_cols = cover_rows * 2;

    // Only recompute when the track or desired grid size changes.
    if (track_ != cover_track_ || cover_cols != cover_cells_w_ ||
        cover_rows != cover_cells_h_) {
      cover_track_ = track_;
      cover_cells_w_ = cover_cols;
      cover_cells_h_ = cover_rows;
      cover_element_ =
          RenderCoverAsBraille(track_, cover_cells_w_, cover_cells_h_);
    }
    // Allocate remaining horizontal space for the bouncy bars.
    int bars_cols = std::max(0, total_w - cover_cols - 2);
    ftxui::Element bars = RenderBouncyBars(cover_rows, bars_cols,
                                           position_seconds_);

    if (bars_cols > 0) {
      title = ftxui::hbox({cover_element_, ftxui::text(" "), bars});
    } else {
      title = cover_element_;
    }
  } else {
    // Either no cover art, or stb_image is not available: show text.
    cover_track_.reset();
    cover_element_ = ftxui::text("");

    std::string line;
    if (track_) {
      line = track_->title + " - " + track_->artist;
    } else {
      line = "";
    }
    title = ftxui::text(line) | ftxui::bold | ftxui::center;
  }
  // Controls row: previous, play/pause, next.
    // Use fixed-size areas for easier clicking, but no borders.
    auto prev_button =
      ftxui::vbox({ftxui::filler(), ftxui::text("⏮") | ftxui::center,
             ftxui::filler()}) |
      ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7) |
      ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3) |
      ftxui::reflect(prev_box_);

  // Draw a larger play/pause icon using multiple text lines instead of a
  // single character. The icons use braille to give a softer shape.
  ftxui::Element play_icon;
  if (playing_) {
    // Pause: two vertical bars with rounded braille edges.
    play_icon = ftxui::vbox({ftxui::text("⣶ ⣶"),
                             ftxui::text("⣿ ⣿"),
                             ftxui::text("⣿ ⣿"),
                             ftxui::text("⣦ ⣦")}) |
                 ftxui::center;
  } else {
    // Play: a right-pointing triangle made from braille dots.
    // Apex on the left, widening to the right over 5 rows.
    play_icon = ftxui::vbox({ftxui::text("⣿⣿⣦⡀   "), ftxui::text("⣿⣿⣿⣿⣿⣦⣄"),
                             ftxui::text("⣿⣿⣿⣿⣿⠿⠋"), ftxui::text("⣿⣿⠟⠉   ")}) |
                ftxui::center;
  }

  auto play_button = play_icon | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 9) |
                     ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 5) |
                     ftxui::reflect(play_box_);

    auto next_button =
      ftxui::vbox({ftxui::filler(), ftxui::text("⏭") | ftxui::center,
             ftxui::filler()}) |
      ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7) |
      ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 3) |
      ftxui::reflect(next_box_);

  auto controls = ftxui::hbox({prev_button, play_button, next_button}) |
                  ftxui::center;

  // Progress bar (0..1). Let it flex horizontally while showing
  // current and total time in MM:SS format.
  float clamped_progress = std::max(0.0f, std::min(progress_, 1.0f));
  auto format_time = [](double seconds) {
    if (seconds <= 0.0) {
      return std::string("00:00");
    }
    int total_sec = static_cast<int>(seconds + 0.5);
    int minutes = total_sec / 60;
    int secs = total_sec % 60;

    std::string s;
    if (minutes < 10)
      s += "0";
    s += std::to_string(minutes);
    s += ":";
    if (secs < 10)
      s += "0";
    s += std::to_string(secs);
    return s;
  };

  auto current_time_text = ftxui::text(format_time(position_seconds_));
  auto total_time_text = ftxui::text(format_time(duration_seconds_));

  auto progress_core = ftxui::gauge(clamped_progress) |
                       ftxui::bgcolor(ftxui::Color::DarkBlue) |
                       ftxui::reflect(progress_box_) | ftxui::flex;

  // The gauge flexes to fill remaining horizontal space between time labels.
  auto progress =
      ftxui::hbox({current_time_text, ftxui::text(" "), progress_core,
                   ftxui::text(" "), total_time_text});

  // Volume slider (0..1) with percentage label.
  float clamped_volume = std::max(0.0f, std::min(volume_, 1.0f));
  int volume_percent = static_cast<int>(clamped_volume * 100.0f + 0.5f);
  auto volume_label =
      ftxui::text("Volume " + std::to_string(volume_percent) + "%");
  auto volume_gauge =
      ftxui::gauge(clamped_volume) | ftxui::reflect(volume_box_) | ftxui::flex;
  auto volume_bar = ftxui::hbox({volume_label, ftxui::text(" "), volume_gauge});

  auto spacer = ftxui::text(" ");
  // Give the cover art/title the flexible space so that when vertical
  // space is limited, the transport controls, progress bar and volume
  // slider remain fully visible and the cover shrinks first.
  auto content = ftxui::vbox({title | ftxui::flex, spacer, controls, spacer,
                              progress, spacer, volume_bar});

  // Align the whole UI to the bottom of the available space and track
  // the Box for sizing decisions on the next frame.
  return ftxui::vbox({ftxui::filler(), content}) | ftxui::flex |
         ftxui::reflect(box_);
}

bool MusicPlayer::handleMouse(ftxui::Event event) {
  const auto m = event.mouse();

  auto make_action = [this](ActionType type) {
    auto a = std::make_unique<Action>();
    a->type = type;
    return a;
  };

  if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed &&
      prev_box_.Contain(m.x, m.y)) {
    auto action = make_action(BACK_IN_QUEUE);
    actions_->push(std::move(action));
    return true;
  }

  if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed &&
      play_box_.Contain(m.x, m.y)) {
    auto action = make_action(PAUSE_RESUME);
    actions_->push(std::move(action));
    return true;
  }

  if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed &&
      next_box_.Contain(m.x, m.y)) {
    auto action = make_action(ADVANCE_IN_QUEUE);
    actions_->push(std::move(action));
    return true;
  }

  // Clicking on the progress bar seeks within the current track.
  if (m.button == ftxui::Mouse::Left && progress_box_.Contain(m.x, m.y)) {
    if (!track_) {
      return false;
    }

    int width = std::max(1, progress_box_.x_max - progress_box_.x_min);
    int offset = std::max(0, std::min(m.x - progress_box_.x_min, width));
    float ratio = static_cast<float>(offset) / static_cast<float>(width);

    auto action = make_action(SEEK);
    action->position =
        static_cast<double>(ratio); // fraction, interpreted by StateWrapper
    actions_->push(std::move(action));
    return true;
  }

  // Clicking on the volume bar adjusts volume.
  if (m.button == ftxui::Mouse::Left && volume_box_.Contain(m.x, m.y)) {
    int width = std::max(1, volume_box_.x_max - volume_box_.x_min);
    int offset = std::max(0, std::min(m.x - volume_box_.x_min, width));
    float ratio = static_cast<float>(offset) / static_cast<float>(width);

    volume_ = ratio;

    auto action = make_action(SET_VOLUME);
    action->volume = ratio;
    actions_->push(std::move(action));
    return true;
  }

  return false;
}

bool MusicPlayer::OnEvent(ftxui::Event event) {
  if (!event.is_mouse()) {
    return false;
  }
  return handleMouse(event);
}
