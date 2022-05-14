 static int check_mtab(const char *progname, const char *devname,
                      const char *dir)
 {
       if (check_newline(progname, devname) || check_newline(progname, dir))
                return EX_USAGE;
        return 0;
 }
