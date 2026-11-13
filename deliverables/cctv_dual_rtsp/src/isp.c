#include "isp.h"
#include "config.h"
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rk_aiq_user_api2_ae.h"
#include "rk_aiq_user_api2_abayertnr_v23.h"
bool request_sensor_fps(const rk_aiq_sys_ctx_t *isp)
{
    Uapi_ExpSwAttrV2_t exposure = {0};
    if (!check("AIQ GetExpSwAttr", CAMERA, rk_aiq_user_api2_ae_getExpSwAttr(isp, &exposure)))
        return false;
    exposure.stAuto.stFrmRate.isFpsFix = true;
    exposure.stAuto.stFrmRate.FpsValue = SENSOR_REQUEST_FPS;
    if (!check("AIQ SetExpSwAttr", CAMERA, rk_aiq_user_api2_ae_setExpSwAttr(isp, exposure)))
        return false;
    printf("AIQ_FPS stage=running requested=%d\n", SENSOR_REQUEST_FPS);
    return true;
}

void report_sensor_timing(const rk_aiq_sys_ctx_t *isp, const char *stage)
{
    Uapi_ExpQueryInfo_t timing = {0};
    if (check("AIQ QueryExpResInfo", CAMERA, rk_aiq_user_api2_ae_queryExpResInfo(isp, &timing)))
        printf("AIQ_TIMING stage=%s reported_fps=%.2f frame_lines=%.0f\n",
               stage, (double)timing.Fps, (double)timing.LinePeriodsPerField);
}

bool disable_temporal_nr(const rk_aiq_sys_ctx_t *isp)
{
    rk_aiq_bayertnr_attrib_v23L_t attr = {0};
    attr.sync.sync_mode = RK_AIQ_UAPI_MODE_SYNC;
    if (!check("Bayer TNR Lite GetAttrib", CAMERA,
               rk_aiq_user_api2_abayertnrV23Lite_GetAttrib(isp, &attr)))
        return false;
    if (attr.eMode != ABAYERTNRV23_OP_MODE_AUTO) {
        fprintf(stderr, "Bayer TNR Lite comparison requires auto mode (mode=%d)\n", attr.eMode);
        return false;
    }
    bool enabled = attr.stAuto.st3DParams.enable;
    attr.stAuto.st3DParams.enable = false;
    attr.sync.sync_mode = RK_AIQ_UAPI_MODE_ASYNC;
    if (!check("Bayer TNR Lite SetAttrib", CAMERA,
               rk_aiq_user_api2_abayertnrV23Lite_SetAttrib(isp, &attr)))
        return false;
    printf("ISP_TNR api=v23_lite auto_enabled_before=%d requested=off\n", enabled);
    return true;
}

void report_bay3d(const char *stage)
{
    FILE *isp_status = fopen("/proc/rkisp-unite0", "r");
    if (isp_status) {
        char line[256];
        while (fgets(line, sizeof line, isp_status)) {
            if (strncmp(line, "BAY3D ", 6) == 0) {
                printf("ISP_HW stage=%s %s", stage, line);
                fclose(isp_status);
                return;
            }
        }
        fclose(isp_status);
    }
    printf("ISP_HW stage=%s BAY3D unavailable\n", stage);
}

bool isp_start(const char *iq_dir, rk_aiq_sys_ctx_t **context, bool *started)
{
    rk_aiq_sys_ctx_t *isp=NULL;
    if (!iq_dir)
        iq_dir = "/etc/iqfiles";
    if (setenv("HDR_MODE", "0", 1)) {
        perror("set HDR_MODE");
        return false;
    }
    rk_aiq_static_info_t sensor = {0};
    if (!check("AIQ Enumerate", CAMERA, rk_aiq_uapi2_sysctl_enumStaticMetas(CAMERA, &sensor)))
        return false;
    *context = isp = rk_aiq_uapi2_sysctl_init(sensor.sensor_info.sensor_name, iq_dir, NULL, NULL);
    if (!isp) {
        fprintf(stderr, "AIQ Init failed with IQ path %s\n", iq_dir);
        return false;
    }
    if (!check("AIQ Prepare", CAMERA, rk_aiq_uapi2_sysctl_prepare(isp, 0, 0, RK_AIQ_WORKING_MODE_NORMAL)) ||
        !check("AIQ Start", CAMERA, rk_aiq_uapi2_sysctl_start(isp)))
        return false;
    *started = true;
    return true;
}
void isp_stop(rk_aiq_sys_ctx_t *context, bool started)
{
    if (started) check("AIQ Stop", CAMERA, rk_aiq_uapi2_sysctl_stop(context, false));
    if (context) rk_aiq_uapi2_sysctl_deinit(context);
}
