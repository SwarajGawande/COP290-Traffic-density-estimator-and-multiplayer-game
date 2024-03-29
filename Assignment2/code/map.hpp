#pragma once

#include "global_dec.hpp"
#include "images.hpp"

enum class map_state {
  block, // #
  food, // .
  counter_food, // C
  enemy_house, // E
  init_p1_pos, // P
  init_p2_pos,
  left_warp_pos, // L
  right_warp_pos, // R
  warp_street, // =
};

class Map {
  SDL_Renderer *renderer_;
  const ImageManager *image_manager_;
  map_state block_[block::count_y][block::count_x];
  unsigned int home_distance_[block::count_y][block::count_x];

 public:
  Map(SDL_Renderer *renderer, const ImageManager *image_manager) noexcept
      : renderer_(renderer), image_manager_(image_manager) {}

  void init(const game_mode mode) noexcept;

  void draw(const unsigned int game_level) const noexcept;

  inline map_state check_state(const Point &p) const noexcept {
    return block_[p.y][p.x];
  }

  inline map_state check_state(const Point &&p) const noexcept {
    return check_state(p);
  }

  inline unsigned int get_home_distance(const Point &p) const noexcept {
    return home_distance_[p.y][p.x];
  }

  inline unsigned int get_home_distance(const Point &&p) const noexcept {
    return get_home_distance(p);
  }

  ~Map() noexcept {}
};
