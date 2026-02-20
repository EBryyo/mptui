#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <track/track.hh>

std::shared_ptr<Track> readMetadata(std::filesystem::path &path) {
  auto track = std::shared_ptr<Track>(new Track);

  // default values
  track->path = path;
  track->title = path.filename();
  track->artist = "Unknown Artist";
  track->index = -1;

  TagLib::FileRef file(path.c_str());
  if (file.isNull()) {
    return track;
  }

  if (TagLib::Tag *tag = file.tag()) {
    if (!tag->title().isEmpty()) {
      track->title = tag->title().to8Bit(true);
    }

    if (!tag->artist().isEmpty()) {
      track->artist = tag->artist().to8Bit(true);
    }

    const unsigned int index = tag->track();
    if (index > 0)
      track->index = index - 1;
  }

  if (auto *props = file.audioProperties()) {
    track->duration = props->lengthInSeconds();
  } else {
    track->duration = -1;
  }

  // Try to load embedded cover art (for MP3 with ID3v2 APIC frames).
  try {
    TagLib::MPEG::File mpeg_file(path.c_str());
    if (auto *id3v2 = mpeg_file.ID3v2Tag()) {
      const auto &frame_map = id3v2->frameListMap();
      auto it = frame_map.find("APIC");
      if (it != frame_map.end() && !it->second.isEmpty()) {
        TagLib::ID3v2::AttachedPictureFrame *chosen = nullptr;

        // Prefer a front cover if available.
        for (auto *frame : it->second) {
          auto *pic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frame);
          if (!pic)
            continue;
          if (pic->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover) {
            chosen = pic;
            break;
          }
          if (!chosen)
            chosen = pic;
        }

        if (chosen) {
          const auto &data = chosen->picture();
          track->image_mime_type = chosen->mimeType().to8Bit(true);
          track->image_data.assign(data.begin(), data.end());
        }
      }
    }
  } catch (...) {
    // Ignore cover art failures; metadata above is still valid.
  }

  if (track->title.empty())
    track->title = path.filename();
  if (track->artist.empty())
    track->artist = "Unknown artist";

  return track;
}
