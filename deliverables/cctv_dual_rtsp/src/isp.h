#ifndef CCTV_ISP_H
#define CCTV_ISP_H
#include <stdbool.h>
#include "rk_aiq_user_api2_sysctl.h"
bool isp_start(const char *iq_dir, rk_aiq_sys_ctx_t **context, bool *started);
void isp_stop(rk_aiq_sys_ctx_t *context, bool started);
bool request_sensor_fps(const rk_aiq_sys_ctx_t *isp);
void report_sensor_timing(const rk_aiq_sys_ctx_t *isp, const char *stage);
bool disable_temporal_nr(const rk_aiq_sys_ctx_t *isp);
void report_bay3d(const char *stage);
#endif
