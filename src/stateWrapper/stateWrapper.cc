#include "action/action.hh"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <stateWrapper/stateWrapper.hh>

void StateWrapper::ProcessActions() {
  // Drain and handle all pending actions.
  while (!actions_->empty()) {
    // transfer ownership to the function scope
    std::unique_ptr<Action> action = std::move(actions_->front());
    actions_->pop();

    if (action->type == SELECT) {
    // If a queue was provided with the selection (e.g. the track belongs
    // to an album), update the TrackQueue to reflect it.
    if (action->queue.has_value()) {
      track_queue_->clear();

      auto queue = action->queue.value();
      track_queue_->setQueue(queue);

      int queue_index = 0;
      if (action->track.has_value()) {
        auto selected_track = action->track.value();
        for (size_t i = 0; i < queue.size(); ++i) {
          if (queue[i].get() == selected_track.get()) {
            queue_index = static_cast<int>(i);
            break;
          }
        }
      }

      track_queue_->setIndex(queue_index);
    }
    if (action->track.has_value()) {
      auto selected_track = action->track.value();
      file_explorer_->SelectTrack(selected_track);

      music_player_->setTrack(selected_track);
      engine_.Play(selected_track->path);
    }
  } else if (action->type == PAUSE_RESUME) {
    if (engine_.Snapshot().playing) {
      engine_.Pause();
    } else {
      engine_.Resume();
    }
  } else if (action->type == ADVANCE_IN_QUEUE) {
    auto queue = track_queue_->getQueue();
    if (queue.empty()) {
      // Nothing to advance to.
    } else {
      int idx = track_queue_->getIndex();
      if (idx < 0) {
        idx = 0;
      } else if (idx + 1 < (int)queue.size()) {
        ++idx;
      } else {
        // At end of queue: do nothing.
      }

      if (idx >= 0 && idx < (int)queue.size()) {
        track_queue_->setIndex(idx);
        auto next_track = queue[idx];
        file_explorer_->SelectTrack(next_track);
        music_player_->setTrack(next_track);
        engine_.Play(next_track->path);
      }
    }
  } else if (action->type == BACK_IN_QUEUE) {
    auto queue = track_queue_->getQueue();
    if (!queue.empty()) {
      auto snapshot = engine_.Snapshot();
      int idx = track_queue_->getIndex();

      if (snapshot.position > 1.0 || idx <= 0) {
        // Restart current (or first) track.
        engine_.Seek(0.0);
      } else {
        // Jump to previous track in the queue.
        --idx;
        track_queue_->setIndex(idx);
        auto prev_track = queue[idx];
        file_explorer_->SelectTrack(prev_track);
        music_player_->setTrack(prev_track);
        engine_.Play(prev_track->path);
      }
    }
  } else if (action->type == SEEK) {
    if (action->position.has_value()) {
      auto snapshot = engine_.Snapshot();
      double target_fraction = action->position.value();
      if (snapshot.duration > 0.0) {
        double target_seconds = snapshot.duration * target_fraction;
        engine_.Seek(target_seconds);
      }
    }
  } else if (action->type == SET_VOLUME) {
    if (action->volume.has_value()) {
      engine_.SetVolume(action->volume.value());
    }
  }
  }

  // Update music player state from the current audio snapshot.
  auto snapshot = engine_.Snapshot();
  float progress = 0.0f;
  if (snapshot.duration > 0.0) {
    progress = static_cast<float>(snapshot.position / snapshot.duration);
  }
  music_player_->setProgress(progress);
  music_player_->setPlaying(snapshot.playing);
  music_player_->setVolume(engine_.GetVolume());
  music_player_->setPosition(snapshot.position);
  music_player_->setDuration(snapshot.duration);
}

bool StateWrapper::OnEvent(ftxui::Event event) {
  // Custom events are used as a periodic tick from a helper thread to
  // advance audio state and process actions on the UI thread.
  if (event == ftxui::Event::Custom) {
    ProcessActions();
    return true;
  }

  if (event == ftxui::Event::Character('q')) {
    closure_();
  }
  bool handled = ftxui::ComponentBase::OnEvent(event);

  // After user input that may have queued actions, process them
  // immediately on the UI thread so the UI stays responsive.
  ProcessActions();

  return handled;
}

ftxui::Element StateWrapper::OnRender() {
  return split->Render() | ftxui::reflect(box_);
}
