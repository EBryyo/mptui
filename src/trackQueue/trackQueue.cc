#include <ftxui/dom/elements.hpp>
#include <trackQueue.hh>

ftxui::Element TrackQueue::OnRender() {
  ftxui::Elements tracks;
  tracks.push_back(ftxui::text("Track list"));
  for (auto &track : track_queue_) {
    auto highlight = track->index == index_ ? ftxui::inverted : ftxui::nothing;
    tracks.push_back(ftxui::text(std::to_string(track->index) + track->title) |
                     highlight);
  }

  return ftxui::vbox(tracks);
}
