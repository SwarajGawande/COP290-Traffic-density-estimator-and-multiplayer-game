#pragma once

#include <SDL2/SDL.h>
#include <math.h>

namespace screen {

enum {
  width = 640,
  height = 480,
  bpp = 32,
  offset_x = 480,
  max_fps = 60,
};

}  

namespace block {

enum {
  size = 20,
  count_x = 24,
  count_y = 24,
};

} 

enum class game_mode {
  single,
  battle,
  help,
};

struct Point {
  int x;
  int y;

  inline bool operator==(const Point &rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }

  inline bool operator!=(const Point &rhs) const noexcept {
    return x != rhs.x || y != rhs.y;
  }

  inline Point operator+(const Point &rhs) const noexcept {
    return Point{x + rhs.x, y + rhs.y};
  }

  inline void operator+=(const Point &rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
  }

 public:
  inline unsigned int distance(const Point &rhs) const noexcept {
    return static_cast<unsigned int>(
        hypot(static_cast<double>(x - rhs.x), static_cast<double>(y - rhs.y)));
  }
  void setx(int i){
  x=i;
  }
  void sety(int i){
  y=i;
  }
  
};
