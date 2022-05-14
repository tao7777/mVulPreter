main(int argc, char **argv)
{
   png_uint_32 opts = FAST_WRITE;
   format_list formats;
 const char *touch = NULL;
 int log_pass = 0;
 int redundant = 0;
 int stride_extra = 0;
 int retval = 0;
 int c;

   init_sRGB_to_d();
#if 0
   init_error_via_linear();
#endif
   format_init(&formats);

 for (c=1; c<argc; ++c)
 {
 const char *arg = argv[c];

 if (strcmp(arg, "--log") == 0)
         log_pass = 1;
 else if (strcmp(arg, "--fresh") == 0)
 {
         memset(gpc_error, 0, sizeof gpc_error);
         memset(gpc_error_via_linear, 0, sizeof gpc_error_via_linear);
 }
 else if (strcmp(arg, "--file") == 0)
#        ifdef PNG_STDIO_SUPPORTED
            opts |= READ_FILE;
#        else
 return 77; /* skipped: no support */
#        endif
 else if (strcmp(arg, "--memory") == 0)
         opts &= ~READ_FILE;
 else if (strcmp(arg, "--stdio") == 0)
#        ifdef PNG_STDIO_SUPPORTED
            opts |= USE_STDIO;
#        else
 return 77; /* skipped: no support */
#        endif
 else if (strcmp(arg, "--name") == 0)
         opts &= ~USE_STDIO;
 else if (strcmp(arg, "--verbose") == 0)
         opts |= VERBOSE;
 else if (strcmp(arg, "--quiet") == 0)
         opts &= ~VERBOSE;
 else if (strcmp(arg, "--preserve") == 0)
         opts |= KEEP_TMPFILES;
 else if (strcmp(arg, "--nopreserve") == 0)
         opts &= ~KEEP_TMPFILES;
 else if (strcmp(arg, "--keep-going") == 0)
         opts |= KEEP_GOING;
 else if (strcmp(arg, "--fast") == 0)
         opts |= FAST_WRITE;
 else if (strcmp(arg, "--slow") == 0)
         opts &= ~FAST_WRITE;
 else if (strcmp(arg, "--accumulate") == 0)
         opts |= ACCUMULATE;
 else if (strcmp(arg, "--redundant") == 0)
         redundant = 1;
 else if (strcmp(arg, "--stop") == 0)
         opts &= ~KEEP_GOING;
 else if (strcmp(arg, "--strict") == 0)
         opts |= STRICT;
 else if (strcmp(arg, "--sRGB-16bit") == 0)
         opts |= sRGB_16BIT;
 else if (strcmp(arg, "--linear-16bit") == 0)
         opts &= ~sRGB_16BIT;
 else if (strcmp(arg, "--tmpfile") == 0)
 {
 if (c+1 < argc)
 {
 if (strlen(argv[++c]) >= sizeof tmpf)
 {
               fflush(stdout);
               fprintf(stderr, "%s: %s is too long for a temp file prefix\n",
                  argv[0], argv[c]);
               exit(99);

             }
 
             /* Safe: checked above */
            strncpy(tmpf, argv[c], sizeof (tmpf)-1);
          }
 
          else
 {
            fflush(stdout);
            fprintf(stderr, "%s: %s requires a temporary file prefix\n",
               argv[0], arg);
            exit(99);
 }
 }
 else if (strcmp(arg, "--touch") == 0)
 {
 if (c+1 < argc)
            touch = argv[++c];

 else
 {
            fflush(stdout);
            fprintf(stderr, "%s: %s requires a file name argument\n",
               argv[0], arg);
            exit(99);
 }
 }
 else if (arg[0] == '+')
 {
         png_uint_32 format = formatof(arg+1);

 if (format > FORMAT_COUNT)
            exit(99);

         format_set(&formats, format);
 }
 else if (arg[0] == '-' && arg[1] != 0 && (arg[1] != '0' || arg[2] != 0))
 {
         fflush(stdout);
         fprintf(stderr, "%s: unknown option: %s\n", argv[0], arg);
         exit(99);
 }
 else
 {
 if (format_is_initial(&formats))
            format_default(&formats, redundant);

 if (arg[0] == '-')
 {
 const int term = (arg[1] == '0' ? 0 : '\n');
 unsigned int ich = 0;

 /* Loop reading files, use a static buffer to simplify this and just
             * stop if the name gets to long.
             */
 static char buffer[4096];

 do
 {
 int ch = getchar();

 /* Don't allow '\0' in file names, and terminate with '\n' or,
                * for -0, just '\0' (use -print0 to find to make this work!)
                */
 if (ch == EOF || ch == term || ch == 0)
 {
                  buffer[ich] = 0;

 if (ich > 0 && !test_one_file(buffer, &formats, opts,
                     stride_extra, log_pass))
                     retval = 1;

 if (ch == EOF)
 break;

                  ich = 0;
 --ich; /* so that the increment below sets it to 0 again */
 }

 else
                  buffer[ich] = (char)ch;
 } while (++ich < sizeof buffer);

 if (ich)
 {
               buffer[32] = 0;
               buffer[4095] = 0;
               fprintf(stderr, "%s...%s: file name too long\n", buffer,
                  buffer+(4096-32));
               exit(99);
 }
 }

 else if (!test_one_file(arg, &formats, opts, stride_extra, log_pass))
            retval = 1;
 }
 }

 if (opts & ACCUMULATE)

    {
       unsigned int in;
 
      printf("/* contrib/libtests/pngstest-errors.h\n");
      printf(" *\n");
      printf(" * BUILT USING:" PNG_HEADER_VERSION_STRING);
      printf(" *\n");
      printf(" * This code is released under the libpng license.\n");
      printf(" * For conditions of distribution and use, see the disclaimer\n");
      printf(" * and license in png.h\n");
      printf(" *\n");
      printf(" * THIS IS A MACHINE GENERATED FILE: do not edit it directly!\n");
      printf(" * Instead run:\n");
      printf(" *\n");
      printf(" *    pngstest --accumulate\n");
      printf(" *\n");
      printf(" * on as many PNG files as possible; at least PNGSuite and\n");
      printf(" * contrib/libtests/testpngs.\n");
      printf(" */\n");

       printf("static png_uint_16 gpc_error[16/*in*/][16/*out*/][4/*a*/] =\n");
       printf("{\n");
       for (in=0; in<16; ++in)
 {
 unsigned int out;
         printf(" { /* input: %s */\n ", format_names[in]);
 for (out=0; out<16; ++out)
 {
 unsigned int alpha;
            printf(" {");
 for (alpha=0; alpha<4; ++alpha)
 {
               printf(" %d", gpc_error[in][out][alpha]);
 if (alpha < 3) putchar(',');
 }
            printf(" }");
 if (out < 15)
 {
               putchar(',');
 if (out % 4 == 3) printf("\n ");
 }
 }
         printf("\n }");

 if (in < 15)
            putchar(',');
 else
            putchar('\n');
 }
      printf("};\n");

      printf("static png_uint_16 gpc_error_via_linear[16][4/*out*/][4] =\n");
      printf("{\n");
 for (in=0; in<16; ++in)
 {
 unsigned int out;
         printf(" { /* input: %s */\n ", format_names[in]);
 for (out=0; out<4; ++out)
 {
 unsigned int alpha;
            printf(" {");
 for (alpha=0; alpha<4; ++alpha)
 {
               printf(" %d", gpc_error_via_linear[in][out][alpha]);
 if (alpha < 3) putchar(',');
 }
            printf(" }");
 if (out < 3)
               putchar(',');
 }
         printf("\n }");

 if (in < 15)
            putchar(',');
 else
            putchar('\n');
 }
      printf("};\n");

      printf("static png_uint_16 gpc_error_to_colormap[8/*i*/][8/*o*/][4] =\n");
      printf("{\n");
 for (in=0; in<8; ++in)
 {
 unsigned int out;
         printf(" { /* input: %s */\n ", format_names[in]);
 for (out=0; out<8; ++out)
 {
 unsigned int alpha;
            printf(" {");
 for (alpha=0; alpha<4; ++alpha)
 {
               printf(" %d", gpc_error_to_colormap[in][out][alpha]);
 if (alpha < 3) putchar(',');
 }
            printf(" }");
 if (out < 7)
 {
               putchar(',');
 if (out % 4 == 3) printf("\n ");
 }
 }
         printf("\n }");

 if (in < 7)
            putchar(',');
 else

             putchar('\n');
       }
       printf("};\n");
      printf("/* END MACHINE GENERATED */\n");
    }
 
    if (retval == 0 && touch != NULL)
 {
 FILE *fsuccess = fopen(touch, "wt");

 if (fsuccess != NULL)
 {
 int error = 0;
         fprintf(fsuccess, "PNG simple API tests succeeded\n");
         fflush(fsuccess);
         error = ferror(fsuccess);

 if (fclose(fsuccess) || error)
 {
            fflush(stdout);
            fprintf(stderr, "%s: write failed\n", touch);
            exit(99);
 }
 }

 else
 {
         fflush(stdout);
         fprintf(stderr, "%s: open failed\n", touch);
         exit(99);
 }
 }

 return retval;
}
