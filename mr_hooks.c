/*
 * This file contains device specific hooks.
 * Always enclose hooks to #if MR_DEVICE_HOOKS >= ver
 * with corresponding hook version!
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>

#include <log.h>

#if MR_DEVICE_HOOKS >= 1
int mrom_hook_after_android_mounts(const char *busybox_path, const char *base_path, int type)
{
    return 0;
}
#endif /* MR_DEVICE_HOOKS >= 1 */


#if MR_DEVICE_HOOKS >= 2
void mrom_hook_before_fb_close(void)
{
}
#endif /* MR_DEVICE_HOOKS >= 2 */


#if MR_DEVICE_HOOKS >= 3
static int read_file(const char *path, char *dest, int dest_size)
{
    int res = 0;
    FILE *f = fopen(path, "r");
    if(!f)
        return res;

    res = fgets(dest, dest_size, f) != NULL;
    fclose(f);
    return res;
}

static int write_file(const char *path, const char *what)
{
    int res = 0;
    FILE *f = fopen(path, "w");
    if(!f)
        return res;

    res = fputs(what, f) >= 0;
    fclose(f);
    return res;
}

static void set_cpu_governor(void)
{
    size_t i;
    char buff[256];
    static const char *governors[] = { "interactive", "ondemand" };

    if(!read_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", buff, sizeof(buff)))
        return;

    if(strncmp(buff, "userspace", 9) != 0)
        return;

    if(!read_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors", buff, sizeof(buff)))
        return;

    write_file("/sys/devices/system/cpu/cpu1/online", "1");
    write_file("/sys/devices/system/cpu/cpu2/online", "1");
    write_file("/sys/devices/system/cpu/cpu3/online", "1");

    for(i = 0; i < sizeof(governors)/sizeof(governors[0]); ++i)
    {
        if(strstr(buff, governors[i]))
        {
            write_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", governors[i]);
            write_file("/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor", governors[i]);
            write_file("/sys/devices/system/cpu/cpu2/cpufreq/scaling_governor", governors[i]);
            write_file("/sys/devices/system/cpu/cpu3/cpufreq/scaling_governor", governors[i]);
            break;
        }
    }

    write_file("/sys/devices/system/cpu/cpufreq/interactive/boost_factor", "2");
    write_file("/sys/devices/system/cpu/cpufreq/interactive/input_boost", "1");
    write_file("/sys/devices/system/cpu/cpufreq/interactive/sustain_load", "80");
    write_file("/sys/module/cpu_tegra3/parameters/auto_hotplug", "1");
    write_file("/sys/module/cpuidle/parameters/lp2_in_idle", "0");
}

void tramp_hook_before_device_init(void)
{
    // 5.1 kernel has userspace as default
    set_cpu_governor();
}
#endif /* MR_DEVICE_HOOKS >= 3 */
