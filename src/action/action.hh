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
};

struct Action {
  ActionType type;
  std::optional<std::shared_ptr<Track>> track;
  std::optional<std::vector<std::shared_ptr<Track>>> queue;
};
