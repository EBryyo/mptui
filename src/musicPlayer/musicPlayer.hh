#pragma once
#include <action/action.hh>
#include <cassert>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>

class MusicPlayer : public ftxui::ComponentBase {
public:
  MusicPlayer(std::queue<std::unique_ptr<Action>> *actions)
      : actions_{actions} {}

  void setTrack(std::shared_ptr<Track> track) { track_ = track; }
  void setProgress(float progress) { progress_ = progress; }
  void setPlaying(bool playing) { playing_ = playing; }
  void setVolume(float volume) { volume_ = volume; }
  void setPosition(double seconds) { position_seconds_ = seconds; }
  void setDuration(double seconds) { duration_seconds_ = seconds; }

  // Component functions
  ftxui::Element OnRender() override;
  bool OnEvent(ftxui::Event event) override;

private:
  bool playing_ = false;
  std::queue<std::unique_ptr<Action>> *actions_;
  std::shared_ptr<Track> track_ = nullptr;
  std::shared_ptr<Track> cover_track_ = nullptr;
  ftxui::Element cover_element_ = ftxui::text("");
  int cover_cells_w_ = 0;
  int cover_cells_h_ = 0;
  float progress_ = 0.0f; // 0..1 fraction
  float volume_ = 1.0f;   // 0..1 fraction

  double position_seconds_ = 0.0;
  double duration_seconds_ = 0.0;

  ftxui::Box box_;
  ftxui::Box prev_box_;
  ftxui::Box play_box_;
  ftxui::Box next_box_;
  ftxui::Box progress_box_;
  ftxui::Box volume_box_;

  bool handleMouse(ftxui::Event event);
};
