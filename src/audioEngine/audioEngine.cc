#include <audioEngine.hh>
#include <stdexcept>

AudioEngine::AudioEngine() {
  if (ma_engine_init(nullptr, &engine_) != MA_SUCCESS) {
    throw std::runtime_error("Failed to initialize miniaudio engine");
  }
}

AudioEngine::~AudioEngine() {
  Stop();
  ma_engine_uninit(&engine_);
}

void AudioEngine::Play(const std::filesystem::path &path) {
  Stop();

  ma_result result = ma_sound_init_from_file(&engine_, path.c_str(), 0, nullptr,
                                             nullptr, &sound_);

  if (result != MA_SUCCESS) {
    has_sound_ = false;
    return;
  }

  ma_sound_start(&sound_);
  has_sound_ = true;
}

void AudioEngine::Pause() {
  if (!has_sound_)
    return;

  ma_sound_stop(&sound_);
}

void AudioEngine::Resume() {
  if (!has_sound_)
    return;
  ma_sound_start(&sound_);
}

void AudioEngine::Stop() {
  if (!has_sound_)
    return;
  ma_sound_uninit(&sound_);
  has_sound_ = false;
}

void AudioEngine::Seek(double seconds) {
  if (!has_sound_)
    return;
  if (seconds < 0.0)
    seconds = 0.0;

  ma_uint32 sample_rate = ma_engine_get_sample_rate(&engine_);
  ma_uint64 frame = static_cast<ma_uint64>(seconds * sample_rate);

  ma_sound_seek_to_pcm_frame(&sound_, frame);
}

AudioState AudioEngine::Snapshot() const {
  AudioState s{};

  if (!has_sound_)
    return s;

  s.playing = ma_sound_is_playing(&sound_);

  ma_uint64 cursor_frames = 0;
  ma_uint64 length_frames = 0;

  ma_sound_get_cursor_in_pcm_frames(&sound_, &cursor_frames);
  ma_sound_get_length_in_pcm_frames(&sound_, &length_frames);

  ma_uint32 sample_rate = ma_engine_get_sample_rate(&engine_);

  s.position = double(cursor_frames) / sample_rate;
  s.duration = double(length_frames) / sample_rate;

  return s;
}

bool AudioEngine::IsFinished() const {
  if (!has_sound_)
    return false;
  if (ma_sound_is_playing(&sound_))
    return false;

  ma_uint64 cursor = 0;
  ma_uint64 length = 0;

  ma_sound_get_cursor_in_pcm_frames(&sound_, &cursor);
  ma_sound_get_length_in_pcm_frames(&sound_, &length);

  if (length == 0)
    return false; // unknown duration (streams)

  return cursor >= length;
}

void AudioEngine::SetVolume(float volume) {
  if (volume < 0.0f)
    volume = 0.0f;
  if (volume > 1.0f)
    volume = 1.0f;
  ma_engine_set_volume(&engine_, volume);
}

float AudioEngine::GetVolume() const {
  return ma_engine_get_volume(const_cast<ma_engine *>(&engine_));
}
