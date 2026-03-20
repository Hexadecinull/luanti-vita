#pragma once

#include <cstdint>

namespace vita_input
{

struct AnalogState {
    float lx, ly;
    float rx, ry;
};

struct TouchPoint {
    float x, y;
    bool active;
};

struct TouchState {
    static constexpr int MAX_TOUCH_POINTS = 6;
    TouchPoint front[MAX_TOUCH_POINTS];
    TouchPoint back[MAX_TOUCH_POINTS];
    int front_count;
    int back_count;
};

void init();
void poll();
void shutdown();

uint32_t get_buttons();
uint32_t get_buttons_down();
uint32_t get_buttons_up();

AnalogState get_analog();
TouchState get_touch();

void inject_sdl_events();

bool is_keyboard_visible();
void show_keyboard(const char *initial_text, int max_len);
void hide_keyboard();
const char *get_keyboard_result();

float get_touch_sensitivity();
void set_touch_sensitivity(float s);

float get_gyro_x();
float get_gyro_y();

}
