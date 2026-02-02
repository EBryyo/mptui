#pragma once
#include <filesystem>
#include <string>

struct Track {
  std::filesystem::path path;
  std::string title;
  std::string artist;
  int duration;
  int index;
};

std::shared_ptr<Track> readMetadata(std::filesystem::path &path);
