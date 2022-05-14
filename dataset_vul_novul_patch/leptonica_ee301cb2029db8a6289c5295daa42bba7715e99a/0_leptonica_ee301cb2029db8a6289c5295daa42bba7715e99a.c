int main(int    argc,
         char **argv)
 {
 char        *filein, *str, *tempfile, *prestring, *outprotos, *protostr;
 const char  *spacestr = " ";
char         buf[L_BUFSIZE];
 l_uint8     *allheaders;
 l_int32      i, maxindex, in_line, nflags, protos_added, firstfile, len, ret;
 size_t       nbytes;
L_BYTEA     *ba, *ba2;
SARRAY      *sa, *safirst;
static char  mainName[] = "xtractprotos";

    if (argc == 1) {
        fprintf(stderr,
                "xtractprotos [-prestring=<string>] [-protos=<where>] "
                "[list of C files]\n"
                "where the prestring is prepended to each prototype, and \n"
                "protos can be either 'inline' or the name of an output "
                "prototype file\n");
        return 1;
    }

    /* ---------------------------------------------------------------- */
    /* Parse input flags and find prestring and outprotos, if requested */
    /* ---------------------------------------------------------------- */
    prestring = outprotos = NULL;
    in_line = FALSE;
    nflags = 0;
    maxindex = L_MIN(3, argc);
    for (i = 1; i < maxindex; i++) {
         if (argv[i][0] == '-') {
             if (!strncmp(argv[i], "-prestring", 10)) {
                 nflags++;
                ret = sscanf(argv[i] + 1, "prestring=%490s", buf);
                 if (ret != 1) {
                     fprintf(stderr, "parse failure for prestring\n");
                     return 1;
                 }
                if ((len = strlen(buf)) > L_BUFSIZE - 3) {
                     L_WARNING("prestring too large; omitting!\n", mainName);
                 } else {
                     buf[len] = ' ';
                    buf[len + 1] = '\0';
                    prestring = stringNew(buf);
                 }
             } else if (!strncmp(argv[i], "-protos", 7)) {
                 nflags++;
                ret = sscanf(argv[i] + 1, "protos=%490s", buf);
                 if (ret != 1) {
                     fprintf(stderr, "parse failure for protos\n");
                     return 1;
                }
                outprotos = stringNew(buf);
                if (!strncmp(outprotos, "inline", 7))
                    in_line = TRUE;
            }
        }
    }

    if (argc - nflags < 2) {
        fprintf(stderr, "no files specified!\n");
        return 1;
    }


    /* ---------------------------------------------------------------- */
    /*                   Generate the prototype string                  */
    /* ---------------------------------------------------------------- */
    ba = l_byteaCreate(500);

         /* First the extern C head */
     sa = sarrayCreate(0);
     sarrayAddString(sa, (char *)"/*", L_COPY);
    snprintf(buf, L_BUFSIZE,
              " *  These prototypes were autogen'd by xtractprotos, v. %s",
              version);
     sarrayAddString(sa, buf, L_COPY);
    sarrayAddString(sa, (char *)" */", L_COPY);
    sarrayAddString(sa, (char *)"#ifdef __cplusplus", L_COPY);
    sarrayAddString(sa, (char *)"extern \"C\" {", L_COPY);
    sarrayAddString(sa, (char *)"#endif  /* __cplusplus */\n", L_COPY);
    str = sarrayToString(sa, 1);
    l_byteaAppendString(ba, str);
    lept_free(str);
    sarrayDestroy(&sa);

        /* Then the prototypes */
    firstfile = 1 + nflags;
    protos_added = FALSE;
    if ((tempfile = l_makeTempFilename()) == NULL) {
        fprintf(stderr, "failure to make a writeable temp file\n");
        return 1;
    }
    for (i = firstfile; i < argc; i++) {
        filein = argv[i];
         len = strlen(filein);
         if (filein[len - 1] == 'h')  /* skip .h files */
             continue;
        snprintf(buf, L_BUFSIZE, "cpp -ansi -DNO_PROTOS %s %s",
                  filein, tempfile);
         ret = system(buf);  /* cpp */
         if (ret) {
            fprintf(stderr, "cpp failure for %s; continuing\n", filein);
            continue;
        }

        if ((str = parseForProtos(tempfile, prestring)) == NULL) {
            fprintf(stderr, "parse failure for %s; continuing\n", filein);
            continue;
        }
        if (strlen(str) > 1) {  /* strlen(str) == 1 is a file without protos */
            l_byteaAppendString(ba, str);
            protos_added = TRUE;
        }
        lept_free(str);
    }
    lept_rmfile(tempfile);
    lept_free(tempfile);

        /* Lastly the extern C tail */
    sa = sarrayCreate(0);
    sarrayAddString(sa, (char *)"\n#ifdef __cplusplus", L_COPY);
    sarrayAddString(sa, (char *)"}", L_COPY);
    sarrayAddString(sa, (char *)"#endif  /* __cplusplus */", L_COPY);
    str = sarrayToString(sa, 1);
    l_byteaAppendString(ba, str);
    lept_free(str);
    sarrayDestroy(&sa);

    protostr = (char *)l_byteaCopyData(ba, &nbytes);
    l_byteaDestroy(&ba);


    /* ---------------------------------------------------------------- */
    /*                       Generate the output                        */
    /* ---------------------------------------------------------------- */
    if (!outprotos) {  /* just write to stdout */
        fprintf(stderr, "%s\n", protostr);
        lept_free(protostr);
        return 0;
    }

        /* If no protos were found, do nothing further */
    if (!protos_added) {
        fprintf(stderr, "No protos found\n");
        lept_free(protostr);
        return 1;
    }

        /* Make the output files */
    ba = l_byteaInitFromFile("allheaders_top.txt");
    if (!in_line) {
        snprintf(buf, sizeof(buf), "#include \"%s\"\n", outprotos);
        l_byteaAppendString(ba, buf);
        l_binaryWrite(outprotos, "w", protostr, nbytes);
    } else {
        l_byteaAppendString(ba, protostr);
    }
    ba2 = l_byteaInitFromFile("allheaders_bot.txt");
    l_byteaJoin(ba, &ba2);
    l_byteaWrite("allheaders.h", ba, 0, 0);
    l_byteaDestroy(&ba);
    lept_free(protostr);
    return 0;
}
