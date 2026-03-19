#ifdef __vita__

#include "settings.h"
#include "vita_platform.h"

void vita_apply_settings_defaults(Settings *settings)
{
    if (!settings) return;

    settings->setDefault("screen_w",                   "960");
    settings->setDefault("screen_h",                   "544");
    settings->setDefault("fullscreen",                 "true");

    settings->setDefault("enable_shaders",             "false");
    settings->setDefault("enable_bumpmapping",         "false");
    settings->setDefault("enable_parallax_occlusion",  "false");
    settings->setDefault("enable_waving_water",        "false");
    settings->setDefault("enable_waving_leaves",       "false");
    settings->setDefault("enable_waving_plants",       "false");
    settings->setDefault("enable_3d_clouds",           "false");
    settings->setDefault("smooth_lighting",            "false");

    settings->setDefault("render_distance",            "3");
    settings->setDefault("max_block_send_distance",    "3");
    settings->setDefault("max_simultaneous_block_sends_per_client", "1");

    settings->setDefault("viewing_range",              "48");
    settings->setDefault("max_objects_per_block",      "16");
    settings->setDefault("fps_max",                    "30");
    settings->setDefault("fps_max_unfocused",          "10");

    settings->setDefault("mesh_buffer_min_draw_range", "0");
    settings->setDefault("client_mesh_chunk",          "1");

    settings->setDefault("enable_minimap",             "false");
    settings->setDefault("minimap_shape_round",        "false");
    settings->setDefault("hud_scaling",                "0.75");
    settings->setDefault("gui_scaling",                "0.75");
    settings->setDefault("font_size",                  "12");

    settings->setDefault("sound_volume",               "0.8");
    settings->setDefault("enable_sound",               "true");

    settings->setDefault("remote_port",                "30000");
    settings->setDefault("server_port",                "30000");
    settings->setDefault("server_address",             "");

    settings->setDefault("chat_font_size",             "10");
    settings->setDefault("chat_message_max_size",      "256");
    settings->setDefault("chat_log_length",            "40");

    settings->setDefault("touchscreen",                "true");
    settings->setDefault("touchtarget",                "true");
    settings->setDefault("touch_sensitivity",          "0.4");
    settings->setDefault("tap_threshold",              "20");

    settings->setDefault("texture_min_size",           "1");
    settings->setDefault("mip_map",                    "false");
    settings->setDefault("anisotropic_filter",         "false");
    settings->setDefault("bilinear_filter",            "false");
    settings->setDefault("trilinear_filter",           "false");
    settings->setDefault("texture_clean_transparent",  "true");
    settings->setDefault("performance_tradeoffs",      "true");

    settings->setDefault("dedicated_server_step",      "0.1");
    settings->setDefault("active_object_send_range_blocks", "2");
    settings->setDefault("active_block_range",         "2");
    settings->setDefault("server_map_save_interval",   "180");

    settings->setDefault("max_users",                  "4");

    settings->setDefault("language",                   "en");
    settings->setDefault("video_driver",               "ogles2");
    settings->setDefault("vsync",                      "true");
}

#endif
