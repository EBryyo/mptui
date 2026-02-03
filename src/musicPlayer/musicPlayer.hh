#pragma once
#include <action/action.hh>
#include <cassert>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

class MusicPlayer : public ftxui::ComponentBase {
public:
  MusicPlayer(std::queue<std::unique_ptr<Action>> *actions)
      : actions_{actions} {}

  void setTrack(std::shared_ptr<Track> track) { track_ = track; }
  // Component functions

  ftxui::Element OnRender() override;

private:
  bool playing_ = false;
  bool shuffle_ = false;
  std::queue<std::unique_ptr<Action>> *actions_;
  std::shared_ptr<Track> track_ = nullptr;
  float progress_ = 0;
};
