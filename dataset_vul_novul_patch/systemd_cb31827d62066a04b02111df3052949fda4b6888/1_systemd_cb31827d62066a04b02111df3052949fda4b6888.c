enum nss_status _nss_mymachines_getgrnam_r(
                const char *name,
                struct group *gr,
                char *buffer, size_t buflen,
                int *errnop) {

        _cleanup_bus_error_free_ sd_bus_error error = SD_BUS_ERROR_NULL;
        _cleanup_bus_message_unref_ sd_bus_message* reply = NULL;
        _cleanup_bus_flush_close_unref_ sd_bus *bus = NULL;
        const char *p, *e, *machine;
        uint32_t mapped;
        uid_t gid;
        size_t l;
        int r;

        assert(name);
        assert(gr);

        p = startswith(name, "vg-");
        if (!p)
                goto not_found;

        e = strrchr(p, '-');
         if (!e || e == p)
                 goto not_found;
 
         r = parse_gid(e + 1, &gid);
         if (r < 0)
                 goto not_found;

        machine = strndupa(p, e - p);
        if (!machine_name_is_valid(machine))
                goto not_found;

        r = sd_bus_open_system(&bus);
        if (r < 0)
                goto fail;

        r = sd_bus_call_method(bus,
                               "org.freedesktop.machine1",
                               "/org/freedesktop/machine1",
                               "org.freedesktop.machine1.Manager",
                               "MapFromMachineGroup",
                               &error,
                               &reply,
                               "su",
                               machine, (uint32_t) gid);
        if (r < 0) {
                if (sd_bus_error_has_name(&error, BUS_ERROR_NO_SUCH_GROUP_MAPPING))
                        goto not_found;

                goto fail;
        }

        r = sd_bus_message_read(reply, "u", &mapped);
        if (r < 0)
                goto fail;

        l = sizeof(char*) + strlen(name) + 1;
        if (buflen < l) {
                *errnop = ENOMEM;
                return NSS_STATUS_TRYAGAIN;
        }

        memzero(buffer, sizeof(char*));
        strcpy(buffer + sizeof(char*), name);

        gr->gr_name = buffer + sizeof(char*);
        gr->gr_gid = gid;
        gr->gr_passwd = (char*) "*"; /* locked */
        gr->gr_mem = (char**) buffer;

        *errnop = 0;
        return NSS_STATUS_SUCCESS;

not_found:
        *errnop = 0;
        return NSS_STATUS_NOTFOUND;

fail:
        *errnop = -r;
        return NSS_STATUS_UNAVAIL;
}
