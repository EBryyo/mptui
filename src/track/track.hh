#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct FileNode;

struct Track {
  std::filesystem::path path;
  std::string title;
  std::string artist;
  int duration;
  int index;
  // Optional embedded cover image metadata loaded from the audio file.
  std::string image_mime_type;           // e.g. "image/jpeg", "image/png"
  std::vector<unsigned char> image_data; // raw image bytes
  FileNode *node = nullptr;
};

std::shared_ptr<Track> readMetadata(std::filesystem::path &path);
