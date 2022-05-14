 int main(int argc, char **argv)
 {
   int summary = 1;  /* Print the error summary at the end */
   int memstats = 0; /* Print memory statistics at the end */
 
    /* Create the given output file on success: */
   const char *touch = NULL;
 
    /* This is an array of standard gamma values (believe it or not I've seen
     * every one of these mentioned somewhere.)
    *
    * In the following list the most useful values are first!
    */
 static double
      gammas[]={2.2, 1.0, 2.2/1.45, 1.8, 1.5, 2.4, 2.5, 2.62, 2.9};

 /* This records the command and arguments: */
 size_t cp = 0;
 char command[1024];

 
    anon_context(&pm.this);
 
   gnu_volatile(summary)
   gnu_volatile(memstats)
   gnu_volatile(touch)

    /* Add appropriate signal handlers, just the ANSI specified ones: */
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
   signal(SIGILL, signal_handler);
   signal(SIGINT, signal_handler);
   signal(SIGSEGV, signal_handler);
   signal(SIGTERM, signal_handler);

#ifdef HAVE_FEENABLEEXCEPT
 /* Only required to enable FP exceptions on platforms where they start off
    * disabled; this is not necessary but if it is not done pngvalid will likely
    * end up ignoring FP conditions that other platforms fault.
    */
   feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif

   modifier_init(&pm);

 /* Preallocate the image buffer, because we know how big it needs to be,
    * note that, for testing purposes, it is deliberately mis-aligned by tag
    * bytes either side.  All rows have an additional five bytes of padding for
    * overwrite checking.
    */
   store_ensure_image(&pm.this, NULL, 2, TRANSFORM_ROWMAX, TRANSFORM_HEIGHTMAX);

 /* Don't give argv[0], it's normally some horrible libtool string: */
   cp = safecat(command, sizeof command, cp, "pngvalid");

 /* Default to error on warning: */
   pm.this.treat_warnings_as_errors = 1;

 /* Default assume_16_bit_calculations appropriately; this tells the checking
    * code that 16-bit arithmetic is used for 8-bit samples when it would make a
    * difference.
    */
   pm.assume_16_bit_calculations = PNG_LIBPNG_VER >= 10700;

 /* Currently 16 bit expansion happens at the end of the pipeline, so the
    * calculations are done in the input bit depth not the output.
    *
    * TODO: fix this
    */
   pm.calculations_use_input_precision = 1U;

 
    /* Store the test gammas */
    pm.gammas = gammas;
   pm.ngammas = ARRAY_SIZE(gammas);
    pm.ngamma_tests = 0; /* default to off */
 
   /* Low bit depth gray images don't do well in the gamma tests, until
    * this is fixed turn them off for some gamma cases:
    */
#  ifdef PNG_WRITE_tRNS_SUPPORTED
      pm.test_tRNS = 1;
#  endif
   pm.test_lbg = PNG_LIBPNG_VER >= 10600;
   pm.test_lbg_gamma_threshold = 1;
   pm.test_lbg_gamma_transform = PNG_LIBPNG_VER >= 10600;
   pm.test_lbg_gamma_sbit = 1;
   pm.test_lbg_gamma_composition = PNG_LIBPNG_VER >= 10700;

    /* And the test encodings */
    pm.encodings = test_encodings;
   pm.nencodings = ARRAY_SIZE(test_encodings);
 
#  if PNG_LIBPNG_VER < 10700
      pm.sbitlow = 8U; /* because libpng doesn't do sBIT below 8! */
#  else
      pm.sbitlow = 1U;
#  endif
 
    /* The following allows results to pass if they correspond to anything in the
     * transformed range [input-.5,input+.5]; this is is required because of the
    * way libpng treates the 16_TO_8 flag when building the gamma tables in
    * releases up to 1.6.0.
    *
    * TODO: review this
    */
   pm.use_input_precision_16to8 = 1U;
   pm.use_input_precision_sbit = 1U; /* because libpng now rounds sBIT */

 /* Some default values (set the behavior for 'make check' here).
    * These values simply control the maximum error permitted in the gamma
    * transformations.  The practial limits for human perception are described
    * below (the setting for maxpc16), however for 8 bit encodings it isn't
    * possible to meet the accepted capabilities of human vision - i.e. 8 bit
    * images can never be good enough, regardless of encoding.
    */
   pm.maxout8 = .1; /* Arithmetic error in *encoded* value */
   pm.maxabs8 = .00005; /* 1/20000 */
   pm.maxcalc8 = 1./255; /* +/-1 in 8 bits for compose errors */
   pm.maxpc8 = .499; /* I.e., .499% fractional error */

    pm.maxout16 = .499;  /* Error in *encoded* value */
    pm.maxabs16 = .00005;/* 1/20000 */
    pm.maxcalc16 =1./65535;/* +/-1 in 16 bits for compose errors */
#  if PNG_LIBPNG_VER < 10700
      pm.maxcalcG = 1./((1<<PNG_MAX_GAMMA_8)-1);
#  else
      pm.maxcalcG = 1./((1<<16)-1);
#  endif
 
    /* NOTE: this is a reasonable perceptual limit. We assume that humans can
     * perceive light level differences of 1% over a 100:1 range, so we need to
    * maintain 1 in 10000 accuracy (in linear light space), which is what the
    * following guarantees.  It also allows significantly higher errors at
    * higher 16 bit values, which is important for performance.  The actual
    * maximum 16 bit error is about +/-1.9 in the fixed point implementation but
    * this is only allowed for values >38149 by the following:
    */
   pm.maxpc16 = .005; /* I.e., 1/200% - 1/20000 */

 /* Now parse the command line options. */
 while (--argc >= 1)
 {
 int catmore = 0; /* Set if the argument has an argument. */

 /* Record each argument for posterity: */
      cp = safecat(command, sizeof command, cp, " ");
      cp = safecat(command, sizeof command, cp, *++argv);

 if (strcmp(*argv, "-v") == 0)
         pm.this.verbose = 1;

 else if (strcmp(*argv, "-l") == 0)
         pm.log = 1;


       else if (strcmp(*argv, "-q") == 0)
          summary = pm.this.verbose = pm.log = 0;
 
      else if (strcmp(*argv, "-w") == 0 ||
               strcmp(*argv, "--strict") == 0)
          pm.this.treat_warnings_as_errors = 0;
 
       else if (strcmp(*argv, "--speed") == 0)
         pm.this.speed = 1, pm.ngamma_tests = pm.ngammas, pm.test_standard = 0,
            summary = 0;

 else if (strcmp(*argv, "--memory") == 0)
         memstats = 1;

 else if (strcmp(*argv, "--size") == 0)
         pm.test_size = 1;

 else if (strcmp(*argv, "--nosize") == 0)
         pm.test_size = 0;

 else if (strcmp(*argv, "--standard") == 0)
         pm.test_standard = 1;

 else if (strcmp(*argv, "--nostandard") == 0)
         pm.test_standard = 0;

 else if (strcmp(*argv, "--transform") == 0)
         pm.test_transform = 1;

 else if (strcmp(*argv, "--notransform") == 0)
         pm.test_transform = 0;

#ifdef PNG_READ_TRANSFORMS_SUPPORTED
 else if (strncmp(*argv, "--transform-disable=",
 sizeof "--transform-disable") == 0)
 {
         pm.test_transform = 1;
         transform_disable(*argv + sizeof "--transform-disable");
 }

 else if (strncmp(*argv, "--transform-enable=",
 sizeof "--transform-enable") == 0)
 {
         pm.test_transform = 1;
         transform_enable(*argv + sizeof "--transform-enable");
 }
#endif /* PNG_READ_TRANSFORMS_SUPPORTED */

 else if (strcmp(*argv, "--gamma") == 0)
 {
 /* Just do two gamma tests here (2.2 and linear) for speed: */
         pm.ngamma_tests = 2U;
         pm.test_gamma_threshold = 1;

          pm.test_gamma_transform = 1;
          pm.test_gamma_sbit = 1;
          pm.test_gamma_scale16 = 1;
         pm.test_gamma_background = 1; /* composition */
          pm.test_gamma_alpha_mode = 1;
          }
 
 else if (strcmp(*argv, "--nogamma") == 0)
         pm.ngamma_tests = 0;

 else if (strcmp(*argv, "--gamma-threshold") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_threshold = 1;

 else if (strcmp(*argv, "--nogamma-threshold") == 0)
         pm.test_gamma_threshold = 0;

 else if (strcmp(*argv, "--gamma-transform") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_transform = 1;

 else if (strcmp(*argv, "--nogamma-transform") == 0)
         pm.test_gamma_transform = 0;

 else if (strcmp(*argv, "--gamma-sbit") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_sbit = 1;

 else if (strcmp(*argv, "--nogamma-sbit") == 0)
         pm.test_gamma_sbit = 0;

 else if (strcmp(*argv, "--gamma-16-to-8") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_scale16 = 1;

 else if (strcmp(*argv, "--nogamma-16-to-8") == 0)
         pm.test_gamma_scale16 = 0;

 else if (strcmp(*argv, "--gamma-background") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_background = 1;

 else if (strcmp(*argv, "--nogamma-background") == 0)
         pm.test_gamma_background = 0;

 else if (strcmp(*argv, "--gamma-alpha-mode") == 0)
         pm.ngamma_tests = 2U, pm.test_gamma_alpha_mode = 1;

 else if (strcmp(*argv, "--nogamma-alpha-mode") == 0)
         pm.test_gamma_alpha_mode = 0;

 else if (strcmp(*argv, "--expand16") == 0)
         pm.test_gamma_expand16 = 1;


       else if (strcmp(*argv, "--noexpand16") == 0)
          pm.test_gamma_expand16 = 0;
 
      else if (strcmp(*argv, "--low-depth-gray") == 0)
         pm.test_lbg = pm.test_lbg_gamma_threshold =
            pm.test_lbg_gamma_transform = pm.test_lbg_gamma_sbit =
            pm.test_lbg_gamma_composition = 1;

      else if (strcmp(*argv, "--nolow-depth-gray") == 0)
         pm.test_lbg = pm.test_lbg_gamma_threshold =
            pm.test_lbg_gamma_transform = pm.test_lbg_gamma_sbit =
            pm.test_lbg_gamma_composition = 0;

#     ifdef PNG_WRITE_tRNS_SUPPORTED
         else if (strcmp(*argv, "--tRNS") == 0)
            pm.test_tRNS = 1;
#     endif

      else if (strcmp(*argv, "--notRNS") == 0)
         pm.test_tRNS = 0;

       else if (strcmp(*argv, "--more-gammas") == 0)
          pm.ngamma_tests = 3U;
 
 else if (strcmp(*argv, "--all-gammas") == 0)
         pm.ngamma_tests = pm.ngammas;

 else if (strcmp(*argv, "--progressive-read") == 0)
         pm.this.progressive = 1;

 else if (strcmp(*argv, "--use-update-info") == 0)
 ++pm.use_update_info; /* Can call multiple times */

 
       else if (strcmp(*argv, "--interlace") == 0)
       {
#        if CAN_WRITE_INTERLACE
             pm.interlace_type = PNG_INTERLACE_ADAM7;
#        else /* !CAN_WRITE_INTERLACE */
             fprintf(stderr, "pngvalid: no write interlace support\n");
             return SKIP;
#        endif /* !CAN_WRITE_INTERLACE */
       }
 
       else if (strcmp(*argv, "--use-input-precision") == 0)
         pm.use_input_precision = 1U;

 else if (strcmp(*argv, "--use-calculation-precision") == 0)
         pm.use_input_precision = 0;

 else if (strcmp(*argv, "--calculations-use-input-precision") == 0)
         pm.calculations_use_input_precision = 1U;

 else if (strcmp(*argv, "--assume-16-bit-calculations") == 0)
         pm.assume_16_bit_calculations = 1U;

 else if (strcmp(*argv, "--calculations-follow-bit-depth") == 0)
         pm.calculations_use_input_precision =
            pm.assume_16_bit_calculations = 0;

 else if (strcmp(*argv, "--exhaustive") == 0)
         pm.test_exhaustive = 1;

 else if (argc > 1 && strcmp(*argv, "--sbitlow") == 0)
 --argc, pm.sbitlow = (png_byte)atoi(*++argv), catmore = 1;

 else if (argc > 1 && strcmp(*argv, "--touch") == 0)
 --argc, touch = *++argv, catmore = 1;

 else if (argc > 1 && strncmp(*argv, "--max", 5) == 0)
 {
 --argc;

 if (strcmp(5+*argv, "abs8") == 0)
            pm.maxabs8 = atof(*++argv);

 else if (strcmp(5+*argv, "abs16") == 0)
            pm.maxabs16 = atof(*++argv);

 else if (strcmp(5+*argv, "calc8") == 0)
            pm.maxcalc8 = atof(*++argv);

 else if (strcmp(5+*argv, "calc16") == 0)
            pm.maxcalc16 = atof(*++argv);

 else if (strcmp(5+*argv, "out8") == 0)
            pm.maxout8 = atof(*++argv);

 else if (strcmp(5+*argv, "out16") == 0)
            pm.maxout16 = atof(*++argv);

 else if (strcmp(5+*argv, "pc8") == 0)
            pm.maxpc8 = atof(*++argv);

 else if (strcmp(5+*argv, "pc16") == 0)
            pm.maxpc16 = atof(*++argv);

 else
 {
            fprintf(stderr, "pngvalid: %s: unknown 'max' option\n", *argv);
            exit(99);
 }

         catmore = 1;
 }

 else if (strcmp(*argv, "--log8") == 0)
 --argc, pm.log8 = atof(*++argv), catmore = 1;

 else if (strcmp(*argv, "--log16") == 0)
 --argc, pm.log16 = atof(*++argv), catmore = 1;

#ifdef PNG_SET_OPTION_SUPPORTED
 else if (strncmp(*argv, "--option=", 9) == 0)
 {
 /* Syntax of the argument is <option>:{on|off} */

          const char *arg = 9+*argv;
          unsigned char option=0, setting=0;
 
#ifdef PNG_ARM_NEON
          if (strncmp(arg, "arm-neon:", 9) == 0)
             option = PNG_ARM_NEON, arg += 9;
 
          else
 #endif
#ifdef PNG_EXTENSIONS
         if (strncmp(arg, "extensions:", 11) == 0)
            option = PNG_EXTENSIONS, arg += 11;

         else
#endif
 #ifdef PNG_MAXIMUM_INFLATE_WINDOW
          if (strncmp(arg, "max-inflate-window:", 19) == 0)
             option = PNG_MAXIMUM_INFLATE_WINDOW, arg += 19;

 else
#endif
 {
            fprintf(stderr, "pngvalid: %s: %s: unknown option\n", *argv, arg);
            exit(99);
 }

 if (strcmp(arg, "off") == 0)
            setting = PNG_OPTION_OFF;

 else if (strcmp(arg, "on") == 0)
            setting = PNG_OPTION_ON;

 else
 {
            fprintf(stderr,
 "pngvalid: %s: %s: unknown setting (use 'on' or 'off')\n",
 *argv, arg);
            exit(99);
 }

         pm.this.options[pm.this.noptions].option = option;
         pm.this.options[pm.this.noptions++].setting = setting;
 }
#endif /* PNG_SET_OPTION_SUPPORTED */

 else
 {
         fprintf(stderr, "pngvalid: %s: unknown argument\n", *argv);
         exit(99);
 }

 if (catmore) /* consumed an extra *argv */
 {
         cp = safecat(command, sizeof command, cp, " ");
         cp = safecat(command, sizeof command, cp, *argv);
 }
 }

 /* If pngvalid is run with no arguments default to a reasonable set of the
    * tests.
    */
 if (pm.test_standard == 0 && pm.test_size == 0 && pm.test_transform == 0 &&
      pm.ngamma_tests == 0)
 {
 /* Make this do all the tests done in the test shell scripts with the same
       * parameters, where possible.  The limitation is that all the progressive
       * read and interlace stuff has to be done in separate runs, so only the
       * basic 'standard' and 'size' tests are done.
       */
      pm.test_standard = 1;
      pm.test_size = 1;
      pm.test_transform = 1;
      pm.ngamma_tests = 2U;
 }

 if (pm.ngamma_tests > 0 &&
      pm.test_gamma_threshold == 0 && pm.test_gamma_transform == 0 &&
      pm.test_gamma_sbit == 0 && pm.test_gamma_scale16 == 0 &&
      pm.test_gamma_background == 0 && pm.test_gamma_alpha_mode == 0)
 {
      pm.test_gamma_threshold = 1;
      pm.test_gamma_transform = 1;
      pm.test_gamma_sbit = 1;
      pm.test_gamma_scale16 = 1;
      pm.test_gamma_background = 1;
      pm.test_gamma_alpha_mode = 1;
 }

 else if (pm.ngamma_tests == 0)
 {
 /* Nothing to test so turn everything off: */
      pm.test_gamma_threshold = 0;
      pm.test_gamma_transform = 0;
      pm.test_gamma_sbit = 0;
      pm.test_gamma_scale16 = 0;
      pm.test_gamma_background = 0;
      pm.test_gamma_alpha_mode = 0;
 }


    Try
    {
       /* Make useful base images */
      make_transform_images(&pm);
 
       /* Perform the standard and gamma tests. */
       if (pm.test_standard)
 {
         perform_interlace_macro_validation();
         perform_formatting_test(&pm.this);
#        ifdef PNG_READ_SUPPORTED
            perform_standard_test(&pm);
#        endif
         perform_error_test(&pm);
 }

 /* Various oddly sized images: */
 if (pm.test_size)
 {
         make_size_images(&pm.this);
#        ifdef PNG_READ_SUPPORTED
            perform_size_test(&pm);
#        endif
 }

#ifdef PNG_READ_TRANSFORMS_SUPPORTED
 /* Combinatorial transforms: */
 if (pm.test_transform)
         perform_transform_test(&pm);
#endif /* PNG_READ_TRANSFORMS_SUPPORTED */

#ifdef PNG_READ_GAMMA_SUPPORTED
 if (pm.ngamma_tests > 0)
         perform_gamma_test(&pm, summary);
#endif
 }

 Catch_anonymous
 {
      fprintf(stderr, "pngvalid: test aborted (probably failed in cleanup)\n");
 if (!pm.this.verbose)
 {
 if (pm.this.error[0] != 0)
            fprintf(stderr, "pngvalid: first error: %s\n", pm.this.error);

         fprintf(stderr, "pngvalid: run with -v to see what happened\n");
 }
      exit(1);
 }

 if (summary)
 {
      printf("%s: %s (%s point arithmetic)\n",
 (pm.this.nerrors || (pm.this.treat_warnings_as_errors &&
            pm.this.nwarnings)) ? "FAIL" : "PASS",
         command,
#if defined(PNG_FLOATING_ARITHMETIC_SUPPORTED) || PNG_LIBPNG_VER < 10500
 "floating"
#else
 "fixed"
#endif
 );
 }

 if (memstats)
 {
      printf("Allocated memory statistics (in bytes):\n"
 "\tread  %lu maximum single, %lu peak, %lu total\n"
 "\twrite %lu maximum single, %lu peak, %lu total\n",
 (unsigned long)pm.this.read_memory_pool.max_max,
 (unsigned long)pm.this.read_memory_pool.max_limit,
 (unsigned long)pm.this.read_memory_pool.max_total,
 (unsigned long)pm.this.write_memory_pool.max_max,
 (unsigned long)pm.this.write_memory_pool.max_limit,
 (unsigned long)pm.this.write_memory_pool.max_total);
 }

 /* Do this here to provoke memory corruption errors in memory not directly
    * allocated by libpng - not a complete test, but better than nothing.
    */
   store_delete(&pm.this);

 /* Error exit if there are any errors, and maybe if there are any
    * warnings.
    */
 if (pm.this.nerrors || (pm.this.treat_warnings_as_errors &&
       pm.this.nwarnings))
 {
 if (!pm.this.verbose)
         fprintf(stderr, "pngvalid: %s\n", pm.this.error);

      fprintf(stderr, "pngvalid: %d errors, %d warnings\n", pm.this.nerrors,
          pm.this.nwarnings);

      exit(1);
 }

 /* Success case. */
 if (touch != NULL)
 {
 FILE *fsuccess = fopen(touch, "wt");

 if (fsuccess != NULL)
 {
 int error = 0;
         fprintf(fsuccess, "PNG validation succeeded\n");
         fflush(fsuccess);
         error = ferror(fsuccess);

 if (fclose(fsuccess) || error)
 {
            fprintf(stderr, "%s: write failed\n", touch);
            exit(1);
 }
 }

 else
 {
         fprintf(stderr, "%s: open failed\n", touch);
         exit(1);
 }
 }

 /* This is required because some very minimal configurations do not use it:
    */
   UNUSED(fail)
 return 0;
}
