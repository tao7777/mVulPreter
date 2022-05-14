 static int check_mtab(const char *progname, const char *devname,
                      const char *dir)
 {
       if (check_newline(progname, devname) == -1 ||
           check_newline(progname, dir) == -1)
                return EX_USAGE;
        return 0;
 }
