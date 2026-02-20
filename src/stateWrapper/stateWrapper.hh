#pragma once
#include <action/action.hh>
#include <audioEngine/audioEngine.hh>
#include <fileExplorer/fileExplorer.hh>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/task.hpp>
#include <chrono>
#include <memory>
#include <musicPlayer/musicPlayer.hh>
#include <track/track.hh>
#include <trackQueue/trackQueue.hh>

#define DEFAULT_EXPLORER_SIZE 45
#define DEFAULT_QUEUE_SIZE 20

class StateWrapper : public ftxui::ComponentBase {
public:
  StateWrapper(std::shared_ptr<FileNode> &root,
               ftxui::ScreenInteractive *screen)
      : screen_{screen}, closure_{screen->ExitLoopClosure()} {
    actions_ = std::make_unique<std::queue<std::unique_ptr<Action>>>();
    file_explorer_ = std::make_shared<FileExplorer>(root, actions_.get());
    music_player_ = std::make_shared<MusicPlayer>(actions_.get());
    track_queue_ = std::make_shared<TrackQueue>(actions_.get());

    ftxui::Component explorer =
        ftxui::Renderer([this] { return file_explorer_->Render(); });
    ftxui::Component tracklist =
        ftxui::Renderer([this] { return track_queue_->Render(); });
    ftxui::Component player =
        ftxui::Renderer([this] { return music_player_->Render(); });
    auto subsplit =
        ftxui::ResizableSplitBottom(tracklist, player, &track_list_size_);

    split = ftxui::ResizableSplitLeft(explorer, subsplit, &explorer_size_);

    // link children
    Add(split);
    subsplit->Add(track_queue_);
    subsplit->Add(music_player_);
    split->Add(file_explorer_);
    split->Add(subsplit);
  };

  // Process pending actions from the queue.
  void ProcessActions();

  // Component functions
  bool OnEvent(ftxui::Event event) override;

  ftxui::Element OnRender() override;

private:
  ftxui::Box box_;
  ftxui::ScreenInteractive *screen_;
  ftxui::Closure closure_;

  std::unique_ptr<std::queue<std::unique_ptr<Action>>>
      actions_; // actions queue
  ftxui::Component split;
  std::shared_ptr<FileExplorer> file_explorer_; // file explorer component
  std::shared_ptr<MusicPlayer> music_player_;   // music player component
  std::shared_ptr<TrackQueue> track_queue_;     // track queue component
  std::shared_ptr<Track> current_track_ = nullptr;

  int explorer_size_ = DEFAULT_EXPLORER_SIZE;
  int track_list_size_ = DEFAULT_QUEUE_SIZE;

  AudioEngine engine_;

  void ChangeTrack();
  void ChangeQueue();
  void AdvanceQueue();
};
