#pragma once
#include <filesystem>
#include <memory>
#include <string>

struct FileNode;

struct Track {
  std::filesystem::path path;
  std::string title;
  std::string artist;
  int duration;
  int index;
  FileNode *node = nullptr;
};

std::shared_ptr<Track> readMetadata(std::filesystem::path &path);
