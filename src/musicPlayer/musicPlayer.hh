#pragma once
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

class MusicPlayer : public ftxui::ComponentBase {
public:
  MusicPlayer() : playing_{false} {}

private:
  bool playing_;
  float progress_ = 0;
};
