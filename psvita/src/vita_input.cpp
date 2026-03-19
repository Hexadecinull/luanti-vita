#include "vita_input.h"

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/motion.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/processmgr.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>

namespace vita_input
{

static constexpr float VITA_SCREEN_W = 960.0f;
static constexpr float VITA_SCREEN_H = 544.0f;
static constexpr float DEADZONE      = 20.0f / 128.0f;
static constexpr float BACK_TOUCH_SCALE = 0.65f;

static SceCtrlData prev_pad = {};
static SceCtrlData curr_pad = {};

static SceTouchData prev_touch[2] = {};
static SceTouchData curr_touch[2] = {};

static TouchState g_touch_state = {};
static AnalogState g_analog     = {};

static float g_touch_sens = 1.0f;

static bool g_kbd_visible   = false;
static bool g_kbd_submitted = false;
static char g_kbd_buffer[512] = {};
static SceUInt16 g_ime_buffer[512] = {};

static float clamp_axis(uint8_t raw)
{
    float v = (static_cast<float>(raw) - 128.0f) / 128.0f;
    if (fabsf(v) < DEADZONE) return 0.0f;
    return v;
}

static SDL_Keycode map_button_to_key(uint32_t btn)
{
    switch (btn) {
        case SCE_CTRL_CROSS:     return SDLK_RETURN;
        case SCE_CTRL_CIRCLE:    return SDLK_ESCAPE;
        case SCE_CTRL_SQUARE:    return SDLK_i;
        case SCE_CTRL_TRIANGLE:  return SDLK_t;
        case SCE_CTRL_UP:        return SDLK_w;
        case SCE_CTRL_DOWN:      return SDLK_s;
        case SCE_CTRL_LEFT:      return SDLK_a;
        case SCE_CTRL_RIGHT:     return SDLK_d;
        case SCE_CTRL_L1:        return SDLK_LSHIFT;
        case SCE_CTRL_R1:        return SDLK_e;
        case SCE_CTRL_L2:        return SDLK_z;
        case SCE_CTRL_R2:        return SDLK_f;
        case SCE_CTRL_L3:        return SDLK_k;
        case SCE_CTRL_R3:        return SDLK_v;
        case SCE_CTRL_START:     return SDLK_ESCAPE;
        case SCE_CTRL_SELECT:    return SDLK_F1;
        default:                 return SDLK_UNKNOWN;
    }
}

static void push_key_event(SDL_Keycode kc, bool down)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type                 = down ? SDL_KEYDOWN : SDL_KEYUP;
    ev.key.state            = down ? SDL_PRESSED : SDL_RELEASED;
    ev.key.keysym.sym       = kc;
    ev.key.keysym.scancode  = SDL_GetScancodeFromKey(kc);
    SDL_PushEvent(&ev);
}

static void push_mouse_button(uint8_t btn, bool down, int x, int y)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type         = down ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
    ev.button.button = btn;
    ev.button.state = down ? SDL_PRESSED : SDL_RELEASED;
    ev.button.x     = x;
    ev.button.y     = y;
    SDL_PushEvent(&ev);
}

static void push_mouse_move(int x, int y)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type    = SDL_MOUSEMOTION;
    ev.motion.x = x;
    ev.motion.y = y;
    SDL_PushEvent(&ev);
}

static void process_buttons()
{
    static const uint32_t BUTTON_LIST[] = {
        SCE_CTRL_CROSS,
        SCE_CTRL_CIRCLE,
        SCE_CTRL_SQUARE,
        SCE_CTRL_TRIANGLE,
        SCE_CTRL_UP,
        SCE_CTRL_DOWN,
        SCE_CTRL_LEFT,
        SCE_CTRL_RIGHT,
        SCE_CTRL_L1,
        SCE_CTRL_R1,
        SCE_CTRL_L2,
        SCE_CTRL_R2,
        SCE_CTRL_L3,
        SCE_CTRL_R3,
        SCE_CTRL_START,
        SCE_CTRL_SELECT,
    };

    uint32_t changed = curr_pad.buttons ^ prev_pad.buttons;

    for (uint32_t btn : BUTTON_LIST) {
        if (!(changed & btn)) continue;
        bool now_down = (curr_pad.buttons & btn) != 0;
        SDL_Keycode kc = map_button_to_key(btn);
        if (kc != SDLK_UNKNOWN)
            push_key_event(kc, now_down);
    }
}

static void process_right_stick_as_mouse()
{
    static float accum_x = 480.0f;
    static float accum_y = 272.0f;

    float rx = clamp_axis(curr_pad.rx);
    float ry = clamp_axis(curr_pad.ry);

    if (fabsf(rx) > 0.0f || fabsf(ry) > 0.0f) {
        float speed = 8.0f * g_touch_sens;
        accum_x += rx * speed;
        accum_y += ry * speed;

        accum_x = fmaxf(0.0f, fminf(VITA_SCREEN_W - 1.0f, accum_x));
        accum_y = fmaxf(0.0f, fminf(VITA_SCREEN_H - 1.0f, accum_y));

        push_mouse_move(static_cast<int>(accum_x), static_cast<int>(accum_y));
    }

    bool r2_down = (curr_pad.buttons & SCE_CTRL_R2) != 0;
    bool r2_prev = (prev_pad.buttons & SCE_CTRL_R2) != 0;
    if (r2_down != r2_prev)
        push_mouse_button(SDL_BUTTON_LEFT, r2_down,
                          static_cast<int>(accum_x), static_cast<int>(accum_y));

    bool l2_down = (curr_pad.buttons & SCE_CTRL_L2) != 0;
    bool l2_prev = (prev_pad.buttons & SCE_CTRL_L2) != 0;
    if (l2_down != l2_prev)
        push_mouse_button(SDL_BUTTON_RIGHT, l2_down,
                          static_cast<int>(accum_x), static_cast<int>(accum_y));
}

static void process_touch_port(int port, TouchPoint *pts, int &count,
                                const SceTouchData &prev, const SceTouchData &cur)
{
    count = 0;
    for (int i = 0; i < cur.reportNum && i < TouchState::MAX_TOUCH_POINTS; ++i) {
        float raw_x = cur.report[i].x / 1920.0f * VITA_SCREEN_W;
        float raw_y = cur.report[i].y / 1088.0f * VITA_SCREEN_H;

        if (port == SCE_TOUCH_PORT_BACK) {
            raw_x *= BACK_TOUCH_SCALE;
            raw_y *= BACK_TOUCH_SCALE;
        }

        pts[i].x      = raw_x;
        pts[i].y      = raw_y;
        pts[i].active = true;
        ++count;

        SDL_Event ev;
        memset(&ev, 0, sizeof(ev));
        ev.type           = SDL_FINGERMOTION;
        ev.tfinger.touchId = static_cast<SDL_TouchID>(port);
        ev.tfinger.fingerId = static_cast<SDL_FingerID>(cur.report[i].id);
        ev.tfinger.x      = raw_x / VITA_SCREEN_W;
        ev.tfinger.y      = raw_y / VITA_SCREEN_H;
        SDL_PushEvent(&ev);
    }
}

static void process_front_touch_for_mouse()
{
    if (curr_touch[SCE_TOUCH_PORT_FRONT].reportNum > 0) {
        const auto &rep = curr_touch[SCE_TOUCH_PORT_FRONT].report[0];
        float mx = rep.x / 1920.0f * VITA_SCREEN_W;
        float my = rep.y / 1088.0f * VITA_SCREEN_H;
        push_mouse_move(static_cast<int>(mx), static_cast<int>(my));

        bool was_touching = prev_touch[SCE_TOUCH_PORT_FRONT].reportNum > 0;
        if (!was_touching)
            push_mouse_button(SDL_BUTTON_LEFT, true, static_cast<int>(mx), static_cast<int>(my));
    } else {
        bool was_touching = prev_touch[SCE_TOUCH_PORT_FRONT].reportNum > 0;
        if (was_touching) {
            const auto &rep = prev_touch[SCE_TOUCH_PORT_FRONT].report[0];
            float mx = rep.x / 1920.0f * VITA_SCREEN_W;
            float my = rep.y / 1088.0f * VITA_SCREEN_H;
            push_mouse_button(SDL_BUTTON_LEFT, false, static_cast<int>(mx), static_cast<int>(my));
        }
    }
}

static void poll_ime()
{
    if (!g_kbd_visible) return;

    SceCommonDialogStatus status = sceImeDialogGetStatus();
    if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
        SceImeDialogResult result;
        memset(&result, 0, sizeof(result));
        sceImeDialogGetResult(&result);

        if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
            int i = 0;
            while (g_ime_buffer[i] && i < 511) {
                g_kbd_buffer[i] = static_cast<char>(g_ime_buffer[i] & 0x7F);
                ++i;
            }
            g_kbd_buffer[i]  = '\0';
            g_kbd_submitted  = true;

            SDL_Event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type         = SDL_TEXTINPUT;
            strncpy(ev.text.text, g_kbd_buffer, SDL_TEXTINPUTEVENT_TEXT_SIZE - 1);
            SDL_PushEvent(&ev);
        }

        sceImeDialogTerm();
        g_kbd_visible = false;
    }
}

void init()
{
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK,  SCE_TOUCH_SAMPLING_STATE_START);
    sceMotionStartSampling();
    memset(&prev_pad,   0, sizeof(prev_pad));
    memset(&curr_pad,   0, sizeof(curr_pad));
    memset(&prev_touch, 0, sizeof(prev_touch));
    memset(&curr_touch, 0, sizeof(curr_touch));
    memset(&g_touch_state, 0, sizeof(g_touch_state));
    memset(&g_analog,      0, sizeof(g_analog));
}

void poll()
{
    prev_pad = curr_pad;
    prev_touch[0] = curr_touch[0];
    prev_touch[1] = curr_touch[1];

    sceCtrlPeekBufferPositive(0, &curr_pad, 1);
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &curr_touch[SCE_TOUCH_PORT_FRONT], 1);
    sceTouchPeek(SCE_TOUCH_PORT_BACK,  &curr_touch[SCE_TOUCH_PORT_BACK],  1);

    g_analog.lx = clamp_axis(curr_pad.lx);
    g_analog.ly = clamp_axis(curr_pad.ly);
    g_analog.rx = clamp_axis(curr_pad.rx);
    g_analog.ry = clamp_axis(curr_pad.ry);

    poll_ime();
}

void inject_sdl_events()
{
    process_buttons();
    process_right_stick_as_mouse();
    process_touch_port(SCE_TOUCH_PORT_FRONT,
                       g_touch_state.front, g_touch_state.front_count,
                       prev_touch[SCE_TOUCH_PORT_FRONT],
                       curr_touch[SCE_TOUCH_PORT_FRONT]);
    process_touch_port(SCE_TOUCH_PORT_BACK,
                       g_touch_state.back, g_touch_state.back_count,
                       prev_touch[SCE_TOUCH_PORT_BACK],
                       curr_touch[SCE_TOUCH_PORT_BACK]);
    process_front_touch_for_mouse();
}

void shutdown()
{
    sceMotionStopSampling();
    if (g_kbd_visible) {
        sceImeDialogTerm();
        g_kbd_visible = false;
    }
}

uint32_t get_buttons()     { return curr_pad.buttons; }
uint32_t get_buttons_down()
{
    return (curr_pad.buttons) & ~(prev_pad.buttons);
}
uint32_t get_buttons_up()
{
    return ~(curr_pad.buttons) & (prev_pad.buttons);
}

AnalogState get_analog()   { return g_analog; }
TouchState  get_touch()    { return g_touch_state; }

bool is_keyboard_visible() { return g_kbd_visible; }

void show_keyboard(const char *initial_text, int max_len)
{
    if (g_kbd_visible) return;

    memset(g_ime_buffer, 0, sizeof(g_ime_buffer));
    if (initial_text) {
        int i = 0;
        while (initial_text[i] && i < 511) {
            g_ime_buffer[i] = static_cast<SceUInt16>(initial_text[i]);
            ++i;
        }
    }

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);

    param.supportedLanguages = SCE_IME_LANGUAGE_ENGLISH;
    param.languagesForced    = SCE_FALSE;
    param.type               = SCE_IME_TYPE_DEFAULT;
    param.option             = SCE_IME_OPTION_NO_ASSISTANCE;
    param.inputTextBuffer    = g_ime_buffer;
    param.maxTextLength      = static_cast<SceUInt32>(
                                   max_len > 511 ? 511 : max_len);

    static SceUInt16 title[] = { 'I','n','p','u','t',0 };
    param.title = title;

    sceImeDialogInit(&param);
    g_kbd_visible   = true;
    g_kbd_submitted = false;
    g_kbd_buffer[0] = '\0';
}

void hide_keyboard()
{
    if (!g_kbd_visible) return;
    sceImeDialogTerm();
    g_kbd_visible = false;
}

const char *get_keyboard_result()
{
    if (!g_kbd_submitted) return nullptr;
    g_kbd_submitted = false;
    return g_kbd_buffer;
}

float get_touch_sensitivity()         { return g_touch_sens; }
void  set_touch_sensitivity(float s)  { g_touch_sens = s; }

float get_gyro_x()
{
    SceMotionSensorState state;
    sceMotionGetSensorState(&state, 1);
    return state.gyro.x;
}

float get_gyro_y()
{
    SceMotionSensorState state;
    sceMotionGetSensorState(&state, 1);
    return state.gyro.y;
}

}
