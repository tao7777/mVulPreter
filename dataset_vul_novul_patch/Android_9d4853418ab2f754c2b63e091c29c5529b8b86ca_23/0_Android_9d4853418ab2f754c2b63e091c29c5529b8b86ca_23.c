main(const int argc, const char * const * const argv)
{
 /* For each file on the command line test it with a range of transforms */
 int option_end, ilog = 0;
 struct display d;

   validate_T();
   display_init(&d);

 for (option_end=1; option_end<argc; ++option_end)
 {
 const char *name = argv[option_end];

 if (strcmp(name, "--verbose") == 0)
         d.options = (d.options & ~LEVEL_MASK) | VERBOSE;

 else if (strcmp(name, "--warnings") == 0)
         d.options = (d.options & ~LEVEL_MASK) | WARNINGS;

 else if (strcmp(name, "--errors") == 0)
         d.options = (d.options & ~LEVEL_MASK) | ERRORS;

 else if (strcmp(name, "--quiet") == 0)
         d.options = (d.options & ~LEVEL_MASK) | QUIET;

 else if (strcmp(name, "--exhaustive") == 0)
         d.options |= EXHAUSTIVE;

 else if (strcmp(name, "--fast") == 0)
         d.options &= ~EXHAUSTIVE;

 else if (strcmp(name, "--strict") == 0)
         d.options |= STRICT;

 else if (strcmp(name, "--relaxed") == 0)
         d.options &= ~STRICT;

 else if (strcmp(name, "--log") == 0)
 {
         ilog = option_end; /* prevent display */
         d.options |= LOG;
 }

 else if (strcmp(name, "--nolog") == 0)
         d.options &= ~LOG;

 else if (strcmp(name, "--continue") == 0)
         d.options |= CONTINUE;

 else if (strcmp(name, "--stop") == 0)
         d.options &= ~CONTINUE;

 else if (strcmp(name, "--skip-bugs") == 0)
         d.options |= SKIP_BUGS;

 else if (strcmp(name, "--test-all") == 0)
         d.options &= ~SKIP_BUGS;

 else if (strcmp(name, "--log-skipped") == 0)
         d.options |= LOG_SKIPPED;

 else if (strcmp(name, "--nolog-skipped") == 0)
         d.options &= ~LOG_SKIPPED;

 else if (strcmp(name, "--find-bad-combos") == 0)
         d.options |= FIND_BAD_COMBOS;


       else if (strcmp(name, "--nofind-bad-combos") == 0)
          d.options &= ~FIND_BAD_COMBOS;
 
      else if (strcmp(name, "--list-combos") == 0)
         d.options |= LIST_COMBOS;

      else if (strcmp(name, "--nolist-combos") == 0)
         d.options &= ~LIST_COMBOS;

       else if (name[0] == '-' && name[1] == '-')
       {
          fprintf(stderr, "pngimage: %s: unknown option\n", name);
 return 99;
 }

 else
 break; /* Not an option */
 }

 {
 int i;
 int errors = 0;

 for (i=option_end; i<argc; ++i)
 {
 {
 int ret = do_test(&d, argv[i]);

 if (ret > QUIET) /* abort on user or internal error */
 return 99;
 }

 /* Here on any return, including failures, except user/internal issues
          */
 {
 const int pass = (d.options & STRICT) ?
               RESULT_STRICT(d.results) : RESULT_RELAXED(d.results);

 if (!pass)
 ++errors;

 if (d.options & LOG)
 {
 int j;

               printf("%s: pngimage ", pass ? "PASS" : "FAIL");

 for (j=1; j<option_end; ++j) if (j != ilog)
                  printf("%s ", argv[j]);

               printf("%s\n", d.filename);
 }
 }


          display_clean(&d);
       }
 
      /* Release allocated memory */
      display_destroy(&d);

       return errors != 0;
    }
 }
