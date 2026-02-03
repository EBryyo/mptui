#pragma once
#include <atomic>
#include <filesystem>
#include <miniaudio/miniaudio.h>
#include <string>

struct AudioState {
  bool playing = false;
  double position = 0.0; // in seconds
  double duration = 0.0; // in seconds
};

class AudioEngine {
public:
  AudioEngine();
  ~AudioEngine();

  void Play(const std::filesystem::path &path);
  void Pause();
  void Resume();
  void Stop();
  void Seek(double seconds);

  AudioState Snapshot() const;
  bool IsFinished() const;

private:
  ma_engine engine_;
  ma_sound sound_;
  std::atomic<bool> has_sound_ = false;
};
