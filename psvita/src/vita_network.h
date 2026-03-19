#pragma once

#ifdef __vita__

#ifdef __cplusplus
extern "C" {
#endif

int  vita_network_init();
void vita_network_shutdown();
bool vita_network_is_connected();

#ifdef __cplusplus
}
#endif

#endif
