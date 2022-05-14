int touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ int fd;
        int r;

        assert(path);

         if (parents)
                 mkdir_parents(path, 0755);
 
        fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY,
                        (mode == 0 || mode == MODE_INVALID) ? 0644 : mode);
         if (fd < 0)
                 return -errno;
 
        if (mode != MODE_INVALID) {
                r = fchmod(fd, mode);
                if (r < 0)
                        return -errno;
        }

        if (uid != UID_INVALID || gid != GID_INVALID) {
                r = fchown(fd, uid, gid);
                if (r < 0)
                        return -errno;
        }

        if (stamp != USEC_INFINITY) {
                struct timespec ts[2];

                timespec_store(&ts[0], stamp);
                ts[1] = ts[0];
                r = futimens(fd, ts);
        } else
                r = futimens(fd, NULL);
        if (r < 0)
                return -errno;

        return 0;
}
