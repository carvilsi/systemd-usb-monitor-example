#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// systemd includes
// for event and device
#include <systemd/sd-event.h>
#include <systemd/sd-device.h>

// just to do my live easer for print usb data
#define print_usb_val(x) printf("%s: %s\n", #x, x)

// the callback for device monitor 
static int monitor_handler(sd_device_monitor *m, sd_device *d, void *userdata);

int main()
{
        int r;
        sd_device_monitor *sddm = NULL;
        // create a new monitor device
        r = sd_device_monitor_new(&sddm);
        // check for errors, sd-device functions returns negative values on errors
        if (r < 0) 
                goto finish;

        // we would like to monitor USB activity
        r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, "usb", "usb_device");
        if (r < 0)
                goto finish;

        // start monitoring and attach a handler function to it
        r = sd_device_monitor_start(sddm, monitor_handler, NULL);
        if (r < 0) 
                goto finish;

        // get the event related with device monitor
        sd_event *event = sd_device_monitor_get_event(sddm);
        // starts the event loop
        r = sd_event_loop(event);
        if (r < 0)
                goto finish;

finish:
        if (r < 0) {
                errno = -r;
                fprintf(stderr, "Error: %m\n");
        }
        
        // stop and unref monitor and event
        sd_device_monitor_stop(sddm);
        sd_device_monitor_unref(sddm);
        sd_event_unref(event);
        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int monitor_handler(sd_device_monitor *m, sd_device *d, void *userdata) 
{
        // Note: failure handling is omitted on this scope for brevity

        sd_device_action_t actions;
        // get the action related with the device
        sd_device_get_action(d, &actions);

        // we are interested only when a USB is connected
        // and print some things about the device.
        if (actions == SD_DEVICE_ADD) {
                                
                const char *sysattr = sd_device_get_sysattr_first(d);
                const char *sysattrvl;
                sd_device_get_sysattr_value(d, sysattr, &sysattrvl);

                // we go throw all the attributes that we could find
                while (sysattr != NULL && sysattrvl != NULL) {
                        if (strcmp(sysattr, "serial") != 0)
                                printf("%s: %s\n", sysattr, sysattrvl);
                        else
                                printf("%s: %x\n", sysattr, sysattrvl);
                        sysattr = sd_device_get_sysattr_next(d);
                        sd_device_get_sysattr_value(d, sysattr, &sysattrvl);
                }

                printf("\n------\n");

                const char *prop, *key;
                key = sd_device_get_property_first(d, &prop);
                const char *propv;
                sd_device_get_property_value(d, key, &propv);

                // we go throw all the propertied that we could find
                while (prop != NULL && propv != NULL) {
                        printf("%s: %s\n", key, propv);
                        key = sd_device_get_property_next(d, &prop);
                        sd_device_get_property_value(d, key, &propv);
                }

                const char *path;
                sd_device_get_syspath(d, &path);
                print_usb_val(path);
        }

        return 0;
}

