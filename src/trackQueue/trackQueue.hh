#pragma once
#include <ftxui/component/component_base.hpp>
#include <track/track.hh>
#include <vector>

class TrackQueue : public ftxui::ComponentBase {
public:
  TrackQueue() : index_(-1) {}

  // clears all current information
  void clear() {
    track_queue_.clear();
    index_ = -1;
  };

  // getters and setters exposed to the state
  void setQueue(std::vector<std::shared_ptr<Track>> queue) {
    track_queue_ = queue;
  }
  std::vector<std::shared_ptr<Track>> getQueue() { return track_queue_; }

  void setIndex(int index) { index_ = index; }
  int getIndex() { return index_; }

  std::shared_ptr<Track> getActiveTrack() {
    return index_ >= 0 && index_ < (int)track_queue_.size()
               ? track_queue_[index_]
               : nullptr;
  }

  // Component functions
  ftxui::Element OnRender() override;

  bool Focusable() const override { return true; }

private:
  std::vector<std::shared_ptr<Track>> track_queue_;
  int index_;
};
