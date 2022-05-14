 static void parse_cfg(int flags, int argc, const char **argv, cfg_t *cfg) {
   int i;
   memset(cfg, 0, sizeof(cfg_t));
   cfg->debug_file = stderr;
 
  for (i = 0; i < argc; i++) {
    if (strncmp(argv[i], "max_devices=", 12) == 0)
      sscanf(argv[i], "max_devices=%u", &cfg->max_devs);
    if (strcmp(argv[i], "manual") == 0)
      cfg->manual = 1;
    if (strcmp(argv[i], "debug") == 0)
      cfg->debug = 1;
    if (strcmp(argv[i], "nouserok") == 0)
      cfg->nouserok = 1;
    if (strcmp(argv[i], "openasuser") == 0)
      cfg->openasuser = 1;
    if (strcmp(argv[i], "alwaysok") == 0)
      cfg->alwaysok = 1;
    if (strcmp(argv[i], "interactive") == 0)
      cfg->interactive = 1;
    if (strcmp(argv[i], "cue") == 0)
      cfg->cue = 1;
    if (strcmp(argv[i], "nodetect") == 0)
      cfg->nodetect = 1;
    if (strncmp(argv[i], "authfile=", 9) == 0)
      cfg->auth_file = argv[i] + 9;
    if (strncmp(argv[i], "authpending_file=", 17) == 0)
      cfg->authpending_file = argv[i] + 17;
    if (strncmp(argv[i], "origin=", 7) == 0)
      cfg->origin = argv[i] + 7;
    if (strncmp(argv[i], "appid=", 6) == 0)
      cfg->appid = argv[i] + 6;
    if (strncmp(argv[i], "prompt=", 7) == 0)
      cfg->prompt = argv[i] + 7;
    if (strncmp (argv[i], "debug_file=", 11) == 0) {
      const char *filename = argv[i] + 11;
      if(strncmp (filename, "stdout", 6) == 0) {
        cfg->debug_file = stdout;
      }
      else if(strncmp (filename, "stderr", 6) == 0) {
        cfg->debug_file = stderr;
      }
      else if( strncmp (filename, "syslog", 6) == 0) {
         cfg->debug_file = (FILE *)-1;
       }
       else {
        struct stat st;
        FILE *file;
        if(lstat(filename, &st) == 0) {
          if(S_ISREG(st.st_mode)) {
            file = fopen(filename, "a");
            if(file != NULL) {
              cfg->debug_file = file;
            }
           }
         }
       }
    }
  }

  if (cfg->debug) {
    D(cfg->debug_file, "called.");
    D(cfg->debug_file, "flags %d argc %d", flags, argc);
    for (i = 0; i < argc; i++) {
      D(cfg->debug_file, "argv[%d]=%s", i, argv[i]);
    }
    D(cfg->debug_file, "max_devices=%d", cfg->max_devs);
    D(cfg->debug_file, "debug=%d", cfg->debug);
    D(cfg->debug_file, "interactive=%d", cfg->interactive);
    D(cfg->debug_file, "cue=%d", cfg->cue);
    D(cfg->debug_file, "nodetect=%d", cfg->nodetect);
    D(cfg->debug_file, "manual=%d", cfg->manual);
    D(cfg->debug_file, "nouserok=%d", cfg->nouserok);
    D(cfg->debug_file, "openasuser=%d", cfg->openasuser);
    D(cfg->debug_file, "alwaysok=%d", cfg->alwaysok);
    D(cfg->debug_file, "authfile=%s", cfg->auth_file ? cfg->auth_file : "(null)");
    D(cfg->debug_file, "authpending_file=%s", cfg->authpending_file ? cfg->authpending_file : "(null)");
    D(cfg->debug_file, "origin=%s", cfg->origin ? cfg->origin : "(null)");
     D(cfg->debug_file, "appid=%s", cfg->appid ? cfg->appid : "(null)");
     D(cfg->debug_file, "prompt=%s", cfg->prompt ? cfg->prompt : "(null)");
   }
 }
