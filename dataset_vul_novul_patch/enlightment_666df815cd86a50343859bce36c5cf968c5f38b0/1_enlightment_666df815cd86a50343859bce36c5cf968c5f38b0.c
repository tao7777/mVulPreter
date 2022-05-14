main(int argc,
     char **argv)
{
   int i, gn;
   int test = 0;
   char *action = NULL, *cmd;
   char *output = NULL;
#ifdef HAVE_EEZE_MOUNT
   Eina_Bool mnt = EINA_FALSE;
   const char *act;
#endif
   gid_t gid, gl[65536], egid;

   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-h")) ||
            (!strcmp(argv[i], "-help")) ||
            (!strcmp(argv[i], "--help")))
          {
             printf(
               "This is an internal tool for Enlightenment.\n"
               "do not use it.\n"
               );
             exit(0);
          }
     }
   if (argc >= 3)
     {
        if ((argc == 3) && (!strcmp(argv[1], "-t")))
          {
             test = 1;
             action = argv[2];
          }
	else if (!strcmp(argv[1], "l2ping"))
	  {
	     action = argv[1];
	     output = argv[2];
	  }
#ifdef HAVE_EEZE_MOUNT
        else
          {
             const char *s;

             s = strrchr(argv[1], '/');
             if ((!s) || (!s[1])) exit(1);  /* eeze always uses complete path */
             s++;
             if (strcmp(s, "mount") && strcmp(s, "umount") && strcmp(s, "eject")) exit(1);
             mnt = EINA_TRUE;
             act = s;
             action = argv[1];
          }
#endif
     }
   else if (argc == 2)
     {
        action = argv[1];
     }
   else
     {
        exit(1);
     }
   if (!action) exit(1);
   fprintf(stderr, "action %s %i\n", action, argc);

   uid = getuid();
   gid = getgid();
   egid = getegid();
   gn = getgroups(65536, gl);
   if (gn < 0)
     {
        printf("ERROR: MEMBER OF MORE THAN 65536 GROUPS\n");
        exit(3);
     }
   if (setuid(0) != 0)
     {
        printf("ERROR: UNABLE TO ASSUME ROOT PRIVILEGES\n");
        exit(5);
     }
   if (setgid(0) != 0)
     {
        printf("ERROR: UNABLE TO ASSUME ROOT GROUP PRIVILEGES\n");
        exit(7);
     }

   eina_init();

   if (!auth_action_ok(action, gid, gl, gn, egid))
     {
        printf("ERROR: ACTION NOT ALLOWED: %s\n", action);
        exit(10);
     }
   /* we can add more levels of auth here */

   /* when mounting, this will match the exact path to the exe,
    * as required in sysactions.conf
    * this is intentionally pedantic for security
    */
   cmd = eina_hash_find(actions, action);
   if (!cmd)
     {
        printf("ERROR: UNDEFINED ACTION: %s\n", action);
        exit(20);
     }

   if (!test && !strcmp(action, "l2ping"))
     {
        char tmp[128];
	double latency;

	latency = e_sys_l2ping(output);

	eina_convert_dtoa(latency, tmp);
	fputs(tmp, stdout);

	return (latency < 0) ? 1 : 0;
     }
   /* sanitize environment */
#ifdef HAVE_UNSETENV
# define NOENV(x) unsetenv(x)
#else
# define NOENV(x)
#endif
   NOENV("IFS");
    /* sanitize environment */
 #ifdef HAVE_UNSETENV
 # define NOENV(x) unsetenv(x)
#else
# define NOENV(x)
#endif
    NOENV("IFS");
    NOENV("LD_PRELOAD");
   NOENV("PYTHONPATH");
    NOENV("LD_LIBRARY_PATH");
 #ifdef HAVE_CLEARENV
     clearenv();
 #endif
   /* set path and ifs to minimal defaults */
    putenv("PATH=/bin:/usr/bin");
    putenv("IFS= \t\n");
   const char *p;
   char *end;
   unsigned long muid;
   Eina_Bool nosuid, nodev, noexec, nuid;

   nosuid = nodev = noexec = nuid = EINA_FALSE;

   /* these are the only possible options which can be present here; check them strictly */
   if (eina_strlcpy(buf, opts, sizeof(buf)) >= sizeof(buf)) return EINA_FALSE;
   for (p = buf; p && p[1]; p = strchr(p + 1, ','))
     {
        if (p[0] == ',') p++;
#define CMP(OPT) \
  if (!strncmp(p, OPT, sizeof(OPT) - 1))

        CMP("nosuid,")
        {
           nosuid = EINA_TRUE;
           continue;
        }
        CMP("nodev,")
        {
           nodev = EINA_TRUE;
           continue;
        }
        CMP("noexec,")
        {
           noexec = EINA_TRUE;
           continue;
        }
        CMP("utf8,") continue;
        CMP("utf8=0,") continue;
        CMP("utf8=1,") continue;
        CMP("iocharset=utf8,") continue;
        CMP("uid=")
        {
           p += 4;
           errno = 0;
           muid = strtoul(p, &end, 10);
           if (muid == ULONG_MAX) return EINA_FALSE;
           if (errno) return EINA_FALSE;
           if (end[0] != ',') return EINA_FALSE;
           if (muid != uid) return EINA_FALSE;
           nuid = EINA_TRUE;
           continue;
        }
        return EINA_FALSE;
     }
   if ((!nosuid) || (!nodev) || (!noexec) || (!nuid)) return EINA_FALSE;
   return EINA_TRUE;
}
