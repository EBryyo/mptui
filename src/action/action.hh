#pragma once
#include <optional>
#include <queue>
#include <track/track.hh>

enum ActionType {
  PLAY,
  STOP,
  PAUSE_RESUME,
  SEEK,
  SELECT,
  SET_QUEUE,
  ADVANCE_IN_QUEUE,
  BACK_IN_QUEUE,
  SET_VOLUME,
};

struct Action {
  ActionType type;
  std::optional<std::shared_ptr<Track>> track;
  std::optional<std::vector<std::shared_ptr<Track>>> queue;
  std::optional<double> position; // for SEEK: absolute seconds
  std::optional<float> volume;    // for SET_VOLUME: linear [0,1]
};
