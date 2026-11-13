#ifndef HEARTBEAT_PLATFORM_H
#define HEARTBEAT_PLATFORM_H
int interface_name(const char *name);
int mac_from_interface(const char *name, char mac[13]);
int wg_up(const char *name);
int rtsp_running(const char *process_name);
#endif
