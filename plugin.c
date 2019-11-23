/**
 * Set/Unset the night-light property according t
 * the play/pause state.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <mpv/client.h>
#include <systemd/sd-bus.h>


int mpv_open_cplugin(mpv_handle *handle) {
        const char * service_name = "org.gnome.SettingsDaemon.Color";
        const char * object_path = "/org/gnome/SettingsDaemon/Color";
        const char * interface = "org.gnome.SettingsDaemon.Color";
        const char * member = "DisabledUntilTomorrow";

        bool initial;
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *m = NULL;
        sd_bus *bus = NULL;
        const char *path;
        int r;

        /* Connect to the system bus */
        r = sd_bus_open_user(&bus);
        if (r < 0) {
                fprintf(stderr, "failed to connect to user bus: %s\n", strerror(-r));
                goto finish;
        }

        r = sd_bus_get_property_trivial(bus,
                        service_name,
                        object_path,
                        interface,
                        member,
                        &error,
                        'b',
                        &initial);

        if (r < 0) {
                fprintf(stderr, "failed to get initial setting: %s\n", error.message);
        }

        while (1) {
                mpv_event *event = mpv_wait_event(handle, -1);
                if (event->event_id == MPV_EVENT_START_FILE) {
                        r = sd_bus_set_property(bus,
                                        service_name,
                                        object_path,
                                        interface,
                                        member,
                                        &error,
                                        "b",
                                        "True");
                }
                if (event->event_id == MPV_EVENT_SHUTDOWN) {
                        r = sd_bus_set_property(bus,
                                        service_name,
                                        object_path,
                                        interface,
                                        member,
                                        &error,
                                        "b",
                                        initial);
                        break;
                }
                if (r < 0) {
                        fprintf(stderr, "DAMN IT: %s\n", error.message);
                }
        }
finish:
        sd_bus_error_free(&error);
        sd_bus_message_unref(m);
        sd_bus_unref(bus);
        return 0;
}
