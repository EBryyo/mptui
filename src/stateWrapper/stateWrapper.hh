#pragma once
#include <action/action.hh>
#include <fileExplorer/fileExplorer.hh>
#include <ftxui/component/component_base.hpp>
#include <memory>
#include <musicPlayer/musicPlayer.hh>
#include <track/track.hh>
#include <trackQueue/trackQueue.hh>

class StateWrapper : public ftxui::ComponentBase {
public:
  StateWrapper(std::shared_ptr<FileNode> &root) {
    actions_ = std::make_shared<std::queue<Action>>();
    file_explorer_ = std::make_shared<FileExplorer>(root);
    music_player_ = std::make_shared<MusicPlayer>();
    track_queue_ = std::make_shared<TrackQueue>();

    // link children
    Add(file_explorer_);
    Add(track_queue_);
  };
  // Process pending actions from the queue.
  void ProcessActions();

  // Component functions

  ftxui::Element OnRender() override;

private:
  std::shared_ptr<std::queue<Action>> actions_; // actions queue
  std::shared_ptr<FileExplorer> file_explorer_; // file explorer component
  std::shared_ptr<MusicPlayer> music_player_;   // music player component
  std::shared_ptr<TrackQueue> track_queue_;     // track queue component
  std::shared_ptr<Track> current_track_ = nullptr;
  int explorer_size_ = 60;
};
