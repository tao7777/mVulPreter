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
   /* pass 1 - just nuke known dangerous env vars brutally if possible via
    * unsetenv(). if you don't have unsetenv... there's pass 2 and 3 */
    NOENV("IFS");
   NOENV("CDPATH");
   NOENV("LOCALDOMAIN");
   NOENV("RES_OPTIONS");
   NOENV("HOSTALIASES");
   NOENV("NLSPATH");
   NOENV("PATH_LOCALE");
   NOENV("COLORTERM");
   NOENV("LANG");
   NOENV("LANGUAGE");
   NOENV("LINGUAS");
   NOENV("TERM");
    NOENV("LD_PRELOAD");
    NOENV("LD_LIBRARY_PATH");
   NOENV("SHLIB_PATH");
   NOENV("LIBPATH");
   NOENV("AUTHSTATE");
   NOENV("DYLD_*");
   NOENV("KRB_CONF*");
   NOENV("KRBCONFDIR");
   NOENV("KRBTKFILE");
   NOENV("KRB5_CONFIG*");
   NOENV("KRB5_KTNAME");
   NOENV("VAR_ACE");
   NOENV("USR_ACE");
   NOENV("DLC_ACE");
   NOENV("TERMINFO");
   NOENV("TERMINFO_DIRS");
   NOENV("TERMPATH");
   NOENV("TERMCAP");
   NOENV("ENV");
   NOENV("BASH_ENV");
   NOENV("PS4");
   NOENV("GLOBIGNORE");
   NOENV("SHELLOPTS");
   NOENV("JAVA_TOOL_OPTIONS");
   NOENV("PERLIO_DEBUG");
   NOENV("PERLLIB");
   NOENV("PERL5LIB");
   NOENV("PERL5OPT");
   NOENV("PERL5DB");
   NOENV("FPATH");
   NOENV("NULLCMD");
   NOENV("READNULLCMD");
   NOENV("ZDOTDIR");
   NOENV("TMPPREFIX");
   NOENV("PYTHONPATH");
   NOENV("PYTHONHOME");
   NOENV("PYTHONINSPECT");
   NOENV("RUBYLIB");
   NOENV("RUBYOPT");
# ifdef HAVE_ENVIRON
   if (environ)
     {
        int again;
        char *tmp, *p;

        /* go over environment array again and again... safely */
        do
          {
             again = 0;

             /* walk through and find first entry that we don't like */
             for (i = 0; environ[i]; i++)
               {
                  /* if it begins with any of these, it's possibly nasty */
                  if ((!strncmp(environ[i], "LD_", 3)) ||
                      (!strncmp(environ[i], "_RLD_", 5)) ||
                      (!strncmp(environ[i], "LC_", 3)) ||
                      (!strncmp(environ[i], "LDR_", 3)))
                    {
                       /* unset it */
                       tmp = strdup(environ[i]);
                       if (!tmp) abort();
                       p = strchr(tmp, '=');
                       if (!p) abort();
                       *p = 0;
                       NOENV(p);
                       free(tmp);
                       /* and mark our do to try again from the start in case
                        * unsetenv changes environ ptr */
                       again = 1;
                       break;
                    }
               }
          }
        while (again);
     }
# endif
#endif

   /* pass 2 - clear entire environment so it doesn't exist at all. if you
    * can't do this... you're possibly in trouble... but the worst is still
    * fixed in pass 3 */
 #ifdef HAVE_CLEARENV
   clearenv();
#else
# ifdef HAVE_ENVIRON
   environ = NULL;
# endif
 #endif

   /* pass 3 - set path and ifs to minimal defaults */
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
