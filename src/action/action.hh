#pragma once
#include <optional>
#include <queue>
#include <track/track.hh>

enum ActionType {
  PLAY,
  PAUSE_RESUME,
  SELECT,
  SET_QUEUE,
  STOP,
};

struct Action {
  ActionType type;
  std::optional<std::shared_ptr<Track>> track;
  std::optional<std::vector<std::shared_ptr<Track>>> queue;
};
