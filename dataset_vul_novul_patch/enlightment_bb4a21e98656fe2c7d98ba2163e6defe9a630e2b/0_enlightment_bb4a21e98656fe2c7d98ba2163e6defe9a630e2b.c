main(int argc, char **argv)
{
   int i, valgrind_mode = 0;
   int valgrind_tool = 0;
   int valgrind_gdbserver = 0;
   char buf[16384], **args, *home;
   char valgrind_path[PATH_MAX] = "";
   const char *valgrind_log = NULL;
   Eina_Bool really_know = EINA_FALSE;
   struct sigaction action;
   pid_t child = -1;
#ifdef E_CSERVE
   pid_t cs_child = -1;
   Eina_Bool cs_use = EINA_FALSE;
#endif
#if !defined(__OpenBSD__) && !defined(__NetBSD__) && !defined(__FreeBSD__) && \
   !defined(__FreeBSD_kernel__) && !(defined (__MACH__) && defined (__APPLE__))
   Eina_Bool restart = EINA_TRUE;
#endif

   unsetenv("NOTIFY_SOCKET");

   /* Setup USR1 to detach from the child process and let it get gdb by advanced users */
   action.sa_sigaction = _sigusr1;
   action.sa_flags = SA_RESETHAND;
   sigemptyset(&action.sa_mask);
   sigaction(SIGUSR1, &action, NULL);

   eina_init();

   /* reexcute myself with dbus-launch if dbus-launch is not running yet */
   if ((!getenv("DBUS_SESSION_BUS_ADDRESS")) &&
       (!getenv("DBUS_LAUNCHD_SESSION_BUS_SOCKET")))
     {
        char **dbus_argv;

        dbus_argv = alloca((argc + 3) * sizeof (char *));
        dbus_argv[0] = "dbus-launch";
        dbus_argv[1] = "--exit-with-session";
        copy_args(dbus_argv + 2, argv, argc);
        dbus_argv[2 + argc] = NULL;
        execvp("dbus-launch", dbus_argv);
     }

   prefix_determine(argv[0]);

   env_set("E_START", argv[0]);

   for (i = 1; i < argc; i++)
     {
        if (!strcmp(argv[i], "-valgrind-gdb"))
          valgrind_gdbserver = 1;
        else if (!strncmp(argv[i], "-valgrind", sizeof("-valgrind") - 1))
          {
             const char *val = argv[i] + sizeof("-valgrind") - 1;

             if (*val == '\0') valgrind_mode = 1;
             else if (*val == '-')
               {
                  val++;
                  if (!strncmp(val, "log-file=", sizeof("log-file=") - 1))
                    {
                       valgrind_log = val + sizeof("log-file=") - 1;
                       if (*valgrind_log == '\0') valgrind_log = NULL;
                    }
               }
             else if (*val == '=')
               {
                  val++;
                  if (!strcmp(val, "all")) valgrind_mode = VALGRIND_MODE_ALL;
                  else valgrind_mode = atoi(val);
               }
             else
               printf("Unknown valgrind option: %s\n", argv[i]);
          }
        else if (!strcmp(argv[i], "-display"))
          {
             i++;
             env_set("DISPLAY", argv[i]);
          }
        else if (!strcmp(argv[i], "-massif"))
          valgrind_tool = 1;
        else if (!strcmp(argv[i], "-callgrind"))
          valgrind_tool = 2;
        else if ((!strcmp(argv[i], "-h")) ||
                 (!strcmp(argv[i], "-help")) ||
                 (!strcmp(argv[i], "--help")))
          {
             printf
             (
               "Options:\n"
               "\t-valgrind[=MODE]\n"
               "\t\tRun enlightenment from inside valgrind, mode is OR of:\n"
               "\t\t   1 = plain valgrind to catch crashes (default)\n"
               "\t\t   2 = trace children (thumbnailer, efm slaves, ...)\n"
               "\t\t   4 = check leak\n"
               "\t\t   8 = show reachable after processes finish.\n"
               "\t\t all = all of above\n"
               "\t-massif\n"
               "\t\tRun enlightenment from inside massif valgrind tool.\n"
               "\t-callgrind\n"
               "\t\tRun enlightenment from inside callgrind valgrind tool.\n"
               "\t-valgrind-log-file=<FILENAME>\n"
               "\t\tSave valgrind log to file, see valgrind's --log-file for details.\n"
               "\n"
               "Please run:\n"
               "\tenlightenment %s\n"
               "for more options.\n",
               argv[i]);
             exit(0);
          }
        else if (!strcmp(argv[i], "-i-really-know-what-i-am-doing-and-accept-full-responsibility-for-it"))
          really_know = EINA_TRUE;
     }

   if (really_know)
     _env_path_append("PATH", eina_prefix_bin_get(pfx));
   else
     _env_path_prepend("PATH", eina_prefix_bin_get(pfx));

   if (valgrind_mode || valgrind_tool)
     {
        if (!find_valgrind(valgrind_path, sizeof(valgrind_path)))
          {
             printf("E - valgrind required but no binary found! Ignoring request.\n");
             valgrind_mode = 0;
          }
     }

   printf("E - PID=%i, valgrind=%d", getpid(), valgrind_mode);
   if (valgrind_mode)
     {
        printf(" valgrind-command='%s'", valgrind_path);
        if (valgrind_log) printf(" valgrind-log-file='%s'", valgrind_log);
     }
   putchar('\n');

   /* mtrack memory tracker support */
   home = getenv("HOME");
   if (home)
     {
        FILE *f;

        /* if you have ~/.e-mtrack, then the tracker will be enabled
         * using the content of this file as the path to the mtrack.so
         * shared object that is the mtrack preload */
        snprintf(buf, sizeof(buf), "%s/.e-mtrack", home);
        f = fopen(buf, "r");
        if (f)
          {
             if (fgets(buf, sizeof(buf), f))
               {
                  int len = strlen(buf);
                  if ((len > 1) && (buf[len - 1] == '\n'))
                    {
                       buf[len - 1] = 0;
                       len--;
                    }
                  env_set("LD_PRELOAD", buf);
                  env_set("MTRACK", "track");
                  env_set("E_START_MTRACK", "track");
                  snprintf(buf, sizeof(buf), "%s/.e-mtrack.log", home);
                  env_set("MTRACK_TRACE_FILE", buf);
               }
             fclose(f);
          }
     }

   /* run e directly now */
   snprintf(buf, sizeof(buf), "%s/enlightenment", eina_prefix_bin_get(pfx));

   args = alloca((argc + 2 + VALGRIND_MAX_ARGS) * sizeof(char *));
   i = valgrind_append(args, valgrind_gdbserver, valgrind_mode, valgrind_tool, valgrind_path, valgrind_log);
   args[i++] = buf;
   copy_args(args + i, argv + 1, argc - 1);
   args[i + argc - 1] = NULL;

   if (valgrind_tool || valgrind_mode)
     really_know = EINA_TRUE;

#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__) || \
   defined(__FreeBSD_kernel__) || (defined (__MACH__) && defined (__APPLE__))
   execv(args[0], args);
#endif

   /* not run at the moment !! */

#if !defined(__OpenBSD__) && !defined(__NetBSD__) && !defined(__FreeBSD__) && \
   !defined(__FreeBSD_kernel__) && !(defined (__MACH__) && defined (__APPLE__))

#ifdef E_CSERVE
   if (getenv("E_CSERVE"))
     {
        cs_use = EINA_TRUE;
        cs_child = _cserve2_start();
     }
#endif

   /* Now looping until */
   while (restart)
     {
        stop_ptrace = EINA_FALSE;

        child = fork();

        if (child < 0) /* failed attempt */
          return -1;
        else if (child == 0)
          {
#ifdef HAVE_SYS_PTRACE_H
             if (!really_know)
               /* in the child */
               ptrace(PT_TRACE_ME, 0, NULL, NULL);
#endif
             execv(args[0], args);
             return 0; /* We failed, 0 mean normal exit from E with no restart or crash so let exit */
          }
        else
          {
             env_set("E_RESTART", "1");
             /* in the parent */
             pid_t result;
             int status;
             Eina_Bool done = EINA_FALSE;

#ifdef HAVE_SYS_PTRACE_H
             if (!really_know)
               ptrace(PT_ATTACH, child, NULL, NULL);
             result = waitpid(child, &status, 0);
             if ((!really_know) && (!stop_ptrace))
               {
                  if (WIFSTOPPED(status))
                    ptrace(PT_CONTINUE, child, NULL, NULL);
               }
#endif
             while (!done)
               {
                  Eina_Bool remember_sigill = EINA_FALSE;
                  Eina_Bool remember_sigusr1 = EINA_FALSE;

                  result = waitpid(child, &status, WNOHANG);
                  if (!result)
                    {
                       /* Wait for evas_cserve2 and E */
                       result = waitpid(-1, &status, 0);
                    }

                  if (result == child)
                    {
                       if ((WIFSTOPPED(status)) && (!stop_ptrace))
                         {
                            char buffer[4096];
                            char *backtrace_str = NULL;
                            siginfo_t sig;
                            int r = 0;
                            int back;

#ifdef HAVE_SYS_PTRACE_H
                            if (!really_know)
                              r = ptrace(PT_GETSIGINFO, child, NULL, &sig);
#endif
                            back = r == 0 &&
                              sig.si_signo != SIGTRAP ? sig.si_signo : 0;

                            if (sig.si_signo == SIGUSR1)
                              {
                                 if (remember_sigill)
                                   remember_sigusr1 = EINA_TRUE;
                              }
                            else if (sig.si_signo == SIGILL)
                              {
                                 remember_sigill = EINA_TRUE;
                              }
                            else
                              {
                                 remember_sigill = EINA_FALSE;
                              }

                            if (r != 0 ||
                                (sig.si_signo != SIGSEGV &&
                                 sig.si_signo != SIGFPE &&
                                 sig.si_signo != SIGABRT))
                              {
#ifdef HAVE_SYS_PTRACE_H
                                 if (!really_know)
                                   ptrace(PT_CONTINUE, child, NULL, back);
#endif
                                 continue;
                              }
#ifdef HAVE_SYS_PTRACE_H
                            if (!really_know)
                              /* E18 should be in pause, we can detach */
                              ptrace(PT_DETACH, child, NULL, back);
#endif
                            /* And call gdb if available */
                            r = 0;
                            if (home)
                               {
                                  /* call e_sys gdb */
                                  snprintf(buffer, 4096,
                                          "gdb %i %s/.e-crashdump.txt",
                                           child,
                                           home);
                                  r = system(buffer);
                                 r = system(buffer);

                                 fprintf(stderr, "called gdb with '%s' = %i\n",
                                         buffer, WEXITSTATUS(r));

                                 snprintf(buffer, 4096,
                                          "%s/.e-crashdump.txt",
                                          home);

                                 backtrace_str = strdup(buffer);
                                 r = WEXITSTATUS(r);
                              }

                            /* call e_alert */
                            snprintf(buffer, 4096,
                                     backtrace_str ?
                                     "%s/enlightenment/utils/enlightenment_alert %i %i '%s' %i" :
                                     "%s/enlightenment/utils/enlightenment_alert %i %i '%s' %i",
                                     eina_prefix_lib_get(pfx),
                                     sig.si_signo == SIGSEGV && remember_sigusr1 ? SIGILL : sig.si_signo,
                                     child,
                                     backtrace_str,
                                     r);
                            r = system(buffer);

                            /* kill e */
                            kill(child, SIGKILL);

                            if (WEXITSTATUS(r) != 1)
                              {
                                 restart = EINA_FALSE;
                              }
                         }
                       else if (!WIFEXITED(status))
                         {
                            done = EINA_TRUE;
                         }
                       else if (stop_ptrace)
                         {
                            done = EINA_TRUE;
                         }
                    }
                  else if (result == -1)
                    {
                       if (errno != EINTR)
                         {
                            done = EINA_TRUE;
                            restart = EINA_FALSE;
                         }
                       else
                         {
                            if (stop_ptrace)
                              {
                                 kill(child, SIGSTOP);
                                 usleep(200000);
#ifdef HAVE_SYS_PTRACE_H
                                 if (!really_know)
                                   ptrace(PT_DETACH, child, NULL, NULL);
#endif
                              }
                         }
                    }
#ifdef E_CSERVE
                  else if (cs_use && (result == cs_child))
                    {
                       if (WIFSIGNALED(status))
                         {
                            printf("E - cserve2 terminated with signal %d\n",
                                   WTERMSIG(status));
                            cs_child = _cserve2_start();
                         }
                       else if (WIFEXITED(status))
                         {
                            printf("E - cserve2 exited with code %d\n",
                                   WEXITSTATUS(status));
                            cs_child = -1;
                         }
                    }
#endif
               }
          }
     }
#endif

#ifdef E_CSERVE
   if (cs_child > 0)
     {
        pid_t result;
        int status;

        alarm(2);
        kill(cs_child, SIGINT);
        result = waitpid(cs_child, &status, 0);
        if (result != cs_child)
          {
             printf("E - cserve2 did not shutdown in 2 seconds, killing!\n");
             kill(cs_child, SIGKILL);
          }
     }
#endif

   return -1;
}
