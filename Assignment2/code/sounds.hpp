#pragma once

#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <memory>

namespace se_type {

enum {
  siren = 0,
  beginning,
  death,
  chomp,
  beat_enemy,
  count,
};

}  // namespace se_type

class MixerManager {
  Mix_Music *music_;
  Mix_Chunk *se_list_[se_type::count];

  inline void load_music(const char *path) noexcept {
    music_ = Mix_LoadMUS(path);
    if (!music_) {
      std::cerr << "error: " << Mix_GetError() << '\n';
      exit(EXIT_FAILURE);
    }
  }

  inline void load_se(const char *path, const unsigned char se_type) noexcept {
    se_list_[se_type] = Mix_LoadWAV(path);
    if (!se_list_[se_type]) {
      std::cerr << "error: " << Mix_GetError() << '\n';
      exit(EXIT_FAILURE);
    }
  }

 public:
  MixerManager() noexcept {
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) !=
        0) {
      std::cerr << Mix_GetError() << '\n';
      exit(EXIT_FAILURE);
    }

    load_music("./resources/sounds/classic_mario.mp3");
    load_se("./resources/sounds/power_mode.wav", se_type::siren);
    load_se("./resources/sounds/mario_start.wav", se_type::beginning);
    load_se("./resources/sounds/mario_dead.wav", se_type::death);
    load_se("./resources/sounds/coin.wav", se_type::chomp);
    load_se("./resources/sounds/hit.wav", se_type::beat_enemy);
  }

  inline Mix_Music *get_music() const noexcept { return music_; }

  inline Mix_Chunk *get_se(const unsigned char se_type) const noexcept {
    return se_list_[se_type];
  }

  ~MixerManager() noexcept {
    Mix_HaltMusic();
    Mix_HaltChannel(-1);
    Mix_FreeMusic(music_);
    for (const auto &se : se_list_) {
      Mix_FreeChunk(se);
    }
    Mix_CloseAudio();
    atexit(Mix_Quit);
  }
};
