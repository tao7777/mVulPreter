enum nss_status _nss_mymachines_getpwnam_r(
                const char *name,
                struct passwd *pwd,
                char *buffer, size_t buflen,
                int *errnop) {

        _cleanup_bus_error_free_ sd_bus_error error = SD_BUS_ERROR_NULL;
        _cleanup_bus_message_unref_ sd_bus_message* reply = NULL;
        _cleanup_bus_flush_close_unref_ sd_bus *bus = NULL;
        const char *p, *e, *machine;
        uint32_t mapped;
        uid_t uid;
        size_t l;
        int r;

        assert(name);
        assert(pwd);

        p = startswith(name, "vu-");
        if (!p)
                goto not_found;

        e = strrchr(p, '-');
         if (!e || e == p)
                 goto not_found;
 
         r = parse_uid(e + 1, &uid);
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
                               "MapFromMachineUser",
                               &error,
                               &reply,
                               "su",
                               machine, (uint32_t) uid);
        if (r < 0) {
                if (sd_bus_error_has_name(&error, BUS_ERROR_NO_SUCH_USER_MAPPING))
                        goto not_found;

                goto fail;
        }

        r = sd_bus_message_read(reply, "u", &mapped);
        if (r < 0)
                goto fail;

        l = strlen(name);
        if (buflen < l+1) {
                *errnop = ENOMEM;
                return NSS_STATUS_TRYAGAIN;
        }

        memcpy(buffer, name, l+1);

        pwd->pw_name = buffer;
        pwd->pw_uid = mapped;
        pwd->pw_gid = 65534; /* nobody */
        pwd->pw_gecos = buffer;
        pwd->pw_passwd = (char*) "*"; /* locked */
        pwd->pw_dir = (char*) "/";
        pwd->pw_shell = (char*) "/sbin/nologin";

        *errnop = 0;
        return NSS_STATUS_SUCCESS;

not_found:
        *errnop = 0;
        return NSS_STATUS_NOTFOUND;

fail:
        *errnop = -r;
        return NSS_STATUS_UNAVAIL;
}
