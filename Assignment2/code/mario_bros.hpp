#pragma once

#include <time.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include "goomba.hpp"
#include "fonts.hpp"
#include "coin.hpp"
#include "keys.hpp"
#include "map.hpp"
#include "sounds.hpp"
#include "mario.hpp"
#include "wipe.hpp"
#include <cstring>
#include <stdio.h>
#include <string>
#include <iostream>
//#define ENET_IMPLEMENTATION
#include <enet/enet.h>
#include<unistd.h>

using namespace std;

struct RGB {
  Uint8 r;
  Uint8 g;
  Uint8 b;
};

namespace rgb {
const RGB black = RGB{0x00, 0x00, 0x00};
const RGB red = RGB{0xff, 0x00, 0x00};
const RGB white = RGB{0xff, 0xff, 0xff};
}  // namespace rgb

class Mario {
  enum class game_state {
    title,
    start,
    clear,
    miss,
    playing,
    gameover,
    pause,
    //waiting,
  };

  const bool fullscreen_mode_;
  const bool debug_mode_;

  SDL_Window *window_;
  SDL_Renderer *renderer_;
  game_state game_state_;
  unsigned int game_level_;
  game_mode game_mode_;
  unsigned int blink_count_;
  unsigned int game_count_;
  bool debug_lose_enemy_;

  std::unique_ptr<ImageManager> image_manager_;
  std::unique_ptr<MixerManager> mixer_manager_;
  std::unique_ptr<Wipe> wipe_;
  std::unique_ptr<InputManager> input_manager_;
  std::unique_ptr<Map> map_;
  std::unique_ptr<Food> food_;
  std::unique_ptr<Enemy> enemy_;
  std::unique_ptr<Player> p1_;
  std::unique_ptr<Player> p2_;
  FontManager font_manager_;
  bool waiting;

  void game_title() noexcept;
  void game_start() noexcept;
  void play_game() noexcept;
  void game_clear() noexcept;
  void game_miss() noexcept;
  void game_over() noexcept;
  void sendPacket(ENetPeer* server);
  void setP2(ENetEvent event);
  
  ENetHost* client = { 0 };
  ENetAddress address ;
  ENetEvent event ;
  ENetPeer* peer = { 0 };

  inline void game_pause() noexcept {
    map_->draw(game_level_);
    food_->draw();
    enemy_->draw();
    p1_->draw(game_mode_);
    p2_->draw(game_mode_);
    draw_score();
    draw_translucence();
    if (input_manager_->edge_key_p(player_type::p1, input_device::space)) {
      game_state_ = game_state::playing;
    }
  }

  inline void draw_text(const unsigned char font_size, const RGB &rgb,
                        const Point &p, const char *str) const noexcept {
    const SDL_Color color = {rgb.r, rgb.g, rgb.b, 255};
    SDL_Surface *font_surface =
        TTF_RenderUTF8_Blended(font_manager_.get(font_size), str, color);
    SDL_Texture *font_texture =
        SDL_CreateTextureFromSurface(renderer_, font_surface);
    const SDL_Rect src = {0, 0, static_cast<Uint16>(font_surface->w),
                          static_cast<Uint16>(font_surface->h)};
    SDL_Rect dst;
    dst.x = static_cast<Sint16>(p.x);
    dst.y = static_cast<Sint16>(p.y);
    SDL_QueryTexture(font_texture, nullptr, nullptr, &dst.w, &dst.h);
    SDL_RenderCopy(renderer_, font_texture, &src, &dst);
    SDL_DestroyTexture(font_texture);
  }

  inline void draw_text(const unsigned char font_size, const RGB &&rgb,
                        const Point &p, const char *str) const noexcept {
    draw_text(font_size, rgb, p, str);
  }

  inline void draw_text(const unsigned char font_size, const RGB &rgb,
                        const Point &&p, const char *str) const noexcept {
    draw_text(font_size, rgb, p, str);
  }

  inline void draw_text(const unsigned char font_size, const RGB &&rgb,
                        const Point &&p, const char *str) const noexcept {
    draw_text(font_size, rgb, p, str);
  }

  inline void draw_score() const noexcept {
    // TODO: divide it into private functions
    // Draw the plate of background.
    {
      SDL_Texture *p_texture = image_manager_->get(image::plate);
      SDL_Rect dst;
      dst.x = 480;
      dst.y = 0;
      SDL_QueryTexture(p_texture, nullptr, nullptr, &dst.w, &dst.h);
      SDL_RenderCopy(renderer_, p_texture, nullptr, &dst);
      SDL_DestroyTexture(p_texture);
    }

    // Draw the score itself.
    {
      const unsigned int x1 = 480 + 8;
      const unsigned int y1 = 480/ 7 + 10;
      const unsigned int x2 = x1 + 40;
      const unsigned int y2 = y1 + 30;
      const unsigned int x3 = x2 + 30;
      const unsigned int y3 = y2;

      std::stringstream score;
      score << "Score:" << p1_->get_score();
      draw_text(1, rgb::white, Point{x1, y1}, score.str().c_str());

      SDL_Texture *p_texture = image_manager_->get(image::p1);
      SDL_Rect src = {block::size, 0, block::size, block::size};
      SDL_Rect dst = {x2, y2, block::size, block::size};
      SDL_RenderCopy(renderer_, p_texture, &src, &dst);
      SDL_DestroyTexture(p_texture);

      std::stringstream life;
      life << "x  " << p1_->get_life();
      draw_text(font_size::x16, rgb::white, Point{x3, y3}, life.str().c_str());

      if (game_mode_ == game_mode::battle) {
        const unsigned int offset_y = 80;
        std::stringstream score;
        score << "Score:" << p2_->get_score();
        draw_text(font_size::x16, rgb::white, Point{x1, y1 + offset_y},
                  score.str().c_str());

        SDL_Texture *p_texture = image_manager_->get(image::p2);
        const SDL_Rect src = {block::size, 0, block::size, block::size};
        const SDL_Rect dst = {x2, y2 + offset_y, block::size, block::size};
        SDL_RenderCopy(renderer_, p_texture, &src, &dst);
        SDL_DestroyTexture(p_texture);

        std::stringstream life;
        life << "x " << p2_->get_life();
        draw_text(font_size::x16, rgb::white, Point{x3, y3 + offset_y},
                  life.str().c_str());
      }
    }

    // Draw the rest time of power mode.
    {
      const unsigned int x = screen::offset_x + 10;
      const unsigned int y = screen::height / 6 * 4;
      if (p1_->get_power_mode()) {
        SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255);
        const SDL_Rect dst = {
            x, y, static_cast<Uint16>(p1_->get_power_mode() / 4), block::size};
        SDL_RenderFillRect(renderer_, &dst);
      }
      if (p2_->get_power_mode()) {
        SDL_SetRenderDrawColor(renderer_, 128, 128, 0, 255);
        const SDL_Rect dst = {x, y + 30,
                              static_cast<Uint16>(p2_->get_power_mode() / 4),
                              block::size};
        SDL_RenderFillRect(renderer_, &dst);
      }
    }
  }

  inline bool poll_event() const noexcept {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          return false;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            return false;
          }
          break;
        default:
          // do nothing
          break;
      }
    }
    return true;
  }

  inline void wait_game() const noexcept {
    static Uint32 pre_count;
    const double wait_time = 1000.0 / screen::max_fps;
    const Uint32 wait_count = (wait_time + 0.5);
    if (pre_count) {
      const Uint32 now_count = SDL_GetTicks();
      const Uint32 interval = now_count - pre_count;
      if (interval < wait_count) {
        const Uint32 delay_time = wait_count - interval;
        SDL_Delay(delay_time);
      }
    }
    pre_count = SDL_GetTicks();
  }

  inline void draw_fps() const noexcept {
    static Uint32 pre_count;
    const Uint32 now_count = SDL_GetTicks();
    if (pre_count) {
      static double frame_rate;
      Uint32 mut_interval = now_count - pre_count;
      if (mut_interval < 1) {
        mut_interval = 1;
      }
      const Uint32 interval = mut_interval;

      if (!(pre_count % 30)) {
        frame_rate = 1000.0 / interval;
      }

      std::stringstream ss;
      ss << "FrameRate[" << std::setprecision(2)
         << std::setiosflags(std::ios::fixed) << frame_rate << "]";
      draw_text(font_size::x16, rgb::white, Point{screen::offset_x + 15, 16},
                ss.str().c_str());
    }
    pre_count = now_count;
  }

  inline void draw_translucence() noexcept {
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    SDL_Surface *trans_surface =
        SDL_CreateRGBSurface(SDL_SWSURFACE, screen::width, screen::height, 32,
                             rmask, gmask, bmask, amask);
    if (trans_surface == nullptr) {
      std::cerr << "CreateRGBSurface failed: " << SDL_GetError() << '\n';
      exit(EXIT_FAILURE);
    }
    SDL_Texture *trans_texture =
        SDL_CreateTextureFromSurface(renderer_, trans_surface);
    SDL_FreeSurface(trans_surface);
    const SDL_Rect dst = {0, 0, screen::width, screen::height};
    SDL_RenderCopy(renderer_, trans_texture, nullptr, &dst);
    SDL_DestroyTexture(trans_texture);
    if (blink_count_ < 30) {
      draw_text(font_size::x36, rgb::white, Point{165, 170}, "P a u s e");
      ++blink_count_;
    } else if (blink_count_ < 60) {
      ++blink_count_;
    } else {
      blink_count_ = 0;
    }
  }

 public:
  Mario(const bool fullscreen_mode, const bool debug_mode) noexcept
      : fullscreen_mode_(fullscreen_mode),
        debug_mode_(debug_mode),
        window_(nullptr),
        renderer_(nullptr),
        game_state_(game_state::title),
        game_mode_(game_mode::single),
        blink_count_(0),
        game_count_(0),
        debug_lose_enemy_(false) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      std::cerr << "error: " << SDL_GetError() << '\n';
      exit(EXIT_FAILURE);
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (fullscreen_mode_) {
      flags |= SDL_WINDOW_FULLSCREEN;
    }
    window_ = SDL_CreateWindow("Super-Mario-Bros-In-A-Maze", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, screen::width,
                               screen::height, flags);
    if (window_ == nullptr) {
      std::cerr << "error: " << SDL_GetError() << '\n';
      exit(EXIT_FAILURE);
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
      std::cerr << "error: " << SDL_GetError() << '\n';
      exit(EXIT_FAILURE);
    }

    image_manager_ = std::make_unique<ImageManager>(renderer_);
    mixer_manager_ = std::make_unique<MixerManager>();
    input_manager_ = std::make_unique<InputManager>();
    wipe_ = std::make_unique<Wipe>(renderer_);
    map_ = std::make_unique<Map>(renderer_, image_manager_.get());
    food_ = std::make_unique<Food>(image_manager_.get(), mixer_manager_.get());
    enemy_ =
        std::make_unique<Enemy>(image_manager_.get(), mixer_manager_.get());
    p1_ = std::make_unique<Player>(player_type::p1, image_manager_.get(),
                                   input_manager_.get());
    p2_ = std::make_unique<Player>(player_type::p2, image_manager_.get(),
                                   input_manager_.get());

    SDL_ShowCursor(SDL_DISABLE);
  }
  void createClient(){
  if (enet_initialize() != 0) {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    exit(EXIT_FAILURE);
  }
  client = enet_host_create(NULL /* create a client host */,
    1 /* only allow 1 outgoing connection */,
    2 /* allow up 2 channels to be used, 0 and 1 */,
    0 /* assume any amount of incoming bandwidth */,
    0 /* assume any amount of outgoing bandwidth */);
  if (client == NULL) {
    fprintf(stderr,
      "An error occurred while trying to create an ENet client host.\n");
    exit(EXIT_FAILURE);
  }
  /* Connect to some.server.net:1234. */
  enet_address_set_host(&address, "127.0.0.1");
  address.port = 7778;
  /* Initiate the connection, allocating the two channels 0 and 1. */
  peer = enet_host_connect(client, &address, 2, 0);
  if (peer == NULL) {
    fprintf(stderr,
      "No available peers for initiating an ENet connection.\n");
    exit(EXIT_FAILURE);
  }
  /* Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service(client, &event, 5000) > 0 &&
    event.type == ENET_EVENT_TYPE_CONNECT) {
    puts("Connection to some.server.net:1234 succeeded.");
  } else {
    /* Either the 5 seconds are up or a disconnect event was */
    /* received. Reset the peer in the event the 5 seconds   */
    /* had run out without any significant event.            */
    enet_peer_reset(peer);
    puts("Connection to some.server.net:1234 failed.");
  }
  }
  
  
  void closeClient(){
  enet_peer_disconnect(peer, 0);

  uint8_t disconnected = false;
  /* Allow up to 3 seconds for the disconnect to succeed
   * and drop any packets received packets.
   */
  while (enet_host_service(client, &event, 1000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE:
          enet_packet_destroy(event.packet);
          break;
      case ENET_EVENT_TYPE_DISCONNECT:
          puts("Disconnection succeeded.");
          disconnected = true;
          break;
      }
  }

  // Drop connection, since disconnection didn't successed
  if (!disconnected) {
      enet_peer_reset(peer);
  }

  enet_host_destroy(client);
  enet_deinitialize();
  }

  inline void run() noexcept {
    int i=0;
    //unsigned int microsecond = 1000;
    waiting=false;
    for (;;) {
      input_manager_->update(debug_mode_);
      switch (game_state_) {
        case game_state::title:
          game_title();
          break;
        case game_state::start:
          game_start();
          break;
        case game_state::playing:
          srand(i);
          //i++;
          play_game();
          break;
        case game_state::clear:
          game_clear();
          break;
        case game_state::miss:
          game_miss();
          break;
        case game_state::gameover:
          game_over();
          break;
        case game_state::pause:
          game_pause();
          break;
      }
      //usleep(3 * microsecond);
      if (!poll_event()) {
        return;
      }
      if (debug_mode_) {
        draw_fps();
      }
      SDL_RenderPresent(renderer_);
      wait_game();
    }
  }

  ~Mario() noexcept {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    atexit(SDL_Quit);
  }
};
