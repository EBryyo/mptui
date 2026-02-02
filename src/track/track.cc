#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
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

  if (track->title.empty())
    track->title = path.filename();
  if (track->artist.empty())
    track->artist = "Unknown artist";

  return track;
}
