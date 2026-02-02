#pragma once
#include <optional>
#include <queue>
#include <track/track.hh>

enum ActionType {
  PLAY,
  PAUSE,
  SELECT,
  SET_QUEUE,
  STOP,
};

struct Action {
  ActionType type;
  std::optional<Track> track;
  int number;
  std::optional<std::unique_ptr<std::queue<Track>>> queue;
};
