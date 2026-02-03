#include <musicPlayer/musicPlayer.hh>

ftxui::Element MusicPlayer::OnRender() {
  return ftxui::vbox(ftxui::text(track_ ? "now playing: " + track_->title +
                                              " by " + track_->artist
                                        : "Not currently playing anything."));
}
