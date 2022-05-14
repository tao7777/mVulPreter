local void process(char *path)
{
    int method = -1;                /* get_header() return value */
    size_t len;                     /* length of base name (minus suffix) */
    struct stat st;                 /* to get file type and mod time */
    /* all compressed suffixes for decoding search, in length order */
    static char *sufs[] = {".z", "-z", "_z", ".Z", ".gz", "-gz", ".zz", "-zz",
                           ".zip", ".ZIP", ".tgz", NULL};

    /* open input file with name in, descriptor ind -- set name and mtime */
    if (path == NULL) {
        strcpy(g.inf, "<stdin>");
        g.ind = 0;
        g.name = NULL;
        g.mtime = g.headis & 2 ?
                  (fstat(g.ind, &st) ? time(NULL) : st.st_mtime) : 0;
        len = 0;
    }
    else {
        /* set input file name (already set if recursed here) */
        if (path != g.inf) {
            strncpy(g.inf, path, sizeof(g.inf));
            if (g.inf[sizeof(g.inf) - 1])
                bail("name too long: ", path);
        }
        len = strlen(g.inf);

        /* try to stat input file -- if not there and decoding, look for that
           name with compressed suffixes */
        if (lstat(g.inf, &st)) {
            if (errno == ENOENT && (g.list || g.decode)) {
                char **try = sufs;
                do {
                    if (*try == NULL || len + strlen(*try) >= sizeof(g.inf))
                        break;
                    strcpy(g.inf + len, *try++);
                    errno = 0;
                } while (lstat(g.inf, &st) && errno == ENOENT);
            }
#ifdef EOVERFLOW
            if (errno == EOVERFLOW || errno == EFBIG)
                bail(g.inf,
                    " too large -- not compiled with large file support");
#endif
            if (errno) {
                g.inf[len] = 0;
                complain("%s does not exist -- skipping", g.inf);
                return;
            }
            len = strlen(g.inf);
        }

        /* only process regular files, but allow symbolic links if -f,
           recurse into directory if -r */
        if ((st.st_mode & S_IFMT) != S_IFREG &&
            (st.st_mode & S_IFMT) != S_IFLNK &&
            (st.st_mode & S_IFMT) != S_IFDIR) {
            complain("%s is a special file or device -- skipping", g.inf);
            return;
        }
        if ((st.st_mode & S_IFMT) == S_IFLNK && !g.force && !g.pipeout) {
            complain("%s is a symbolic link -- skipping", g.inf);
            return;
        }
        if ((st.st_mode & S_IFMT) == S_IFDIR && !g.recurse) {
            complain("%s is a directory -- skipping", g.inf);
            return;
        }

        /* recurse into directory (assumes Unix) */
        if ((st.st_mode & S_IFMT) == S_IFDIR) {
            char *roll, *item, *cut, *base, *bigger;
            size_t len, hold;
            DIR *here;
            struct dirent *next;

            /* accumulate list of entries (need to do this, since readdir()
               behavior not defined if directory modified between calls) */
            here = opendir(g.inf);
            if (here == NULL)
                return;
            hold = 512;
            roll = MALLOC(hold);
            if (roll == NULL)
                bail("not enough memory", "");
            *roll = 0;
            item = roll;
            while ((next = readdir(here)) != NULL) {
                if (next->d_name[0] == 0 ||
                    (next->d_name[0] == '.' && (next->d_name[1] == 0 ||
                     (next->d_name[1] == '.' && next->d_name[2] == 0))))
                    continue;
                len = strlen(next->d_name) + 1;
                if (item + len + 1 > roll + hold) {
                    do {                    /* make roll bigger */
                        hold <<= 1;
                    } while (item + len + 1 > roll + hold);
                    bigger = REALLOC(roll, hold);
                    if (bigger == NULL) {
                        FREE(roll);
                        bail("not enough memory", "");
                    }
                    item = bigger + (item - roll);
                    roll = bigger;
                }
                strcpy(item, next->d_name);
                item += len;
                *item = 0;
            }
            closedir(here);

            /* run process() for each entry in the directory */
            cut = base = g.inf + strlen(g.inf);
            if (base > g.inf && base[-1] != (unsigned char)'/') {
                if ((size_t)(base - g.inf) >= sizeof(g.inf))
                    bail("path too long", g.inf);
                *base++ = '/';
            }
            item = roll;
            while (*item) {
                strncpy(base, item, sizeof(g.inf) - (base - g.inf));
                if (g.inf[sizeof(g.inf) - 1]) {
                    strcpy(g.inf + (sizeof(g.inf) - 4), "...");
                    bail("path too long: ", g.inf);
                }
                process(g.inf);
                item += strlen(item) + 1;
            }
            *cut = 0;

            /* release list of entries */
            FREE(roll);
            return;
        }

        /* don't compress .gz (or provided suffix) files, unless -f */
        if (!(g.force || g.list || g.decode) && len >= strlen(g.sufx) &&
                strcmp(g.inf + len - strlen(g.sufx), g.sufx) == 0) {
            complain("%s ends with %s -- skipping", g.inf, g.sufx);
            return;
        }

        /* create output file only if input file has compressed suffix */
        if (g.decode == 1 && !g.pipeout && !g.list) {
            int suf = compressed_suffix(g.inf);
            if (suf == 0) {
                complain("%s does not have compressed suffix -- skipping",
                         g.inf);
                return;
            }
            len -= suf;
        }

        /* open input file */
        g.ind = open(g.inf, O_RDONLY, 0);
        if (g.ind < 0)
            bail("read error on ", g.inf);

        /* prepare gzip header information for compression */
        g.name = g.headis & 1 ? justname(g.inf) : NULL;
        g.mtime = g.headis & 2 ? st.st_mtime : 0;
    }
    SET_BINARY_MODE(g.ind);

    /* if decoding or testing, try to read gzip header */
    g.hname = NULL;
    if (g.decode) {
        in_init();
        method = get_header(1);
        if (method != 8 && method != 257 &&
                /* gzip -cdf acts like cat on uncompressed input */
                !(method == -2 && g.force && g.pipeout && g.decode != 2 &&
                  !g.list)) {
            RELEASE(g.hname);
            if (g.ind != 0)
                close(g.ind);
            if (method != -1)
                complain(method < 0 ? "%s is not compressed -- skipping" :
                         "%s has unknown compression method -- skipping",
                         g.inf);
            return;
        }

        /* if requested, test input file (possibly a special list) */
        if (g.decode == 2) {
            if (method == 8)
                infchk();
            else {
                unlzw();
                if (g.list) {
                    g.in_tot -= 3;
                    show_info(method, 0, g.out_tot, 0);
                }
            }
            RELEASE(g.hname);
            if (g.ind != 0)
                close(g.ind);
            return;
        }
    }

    /* if requested, just list information about input file */
    if (g.list) {
        list_info();
        RELEASE(g.hname);
        if (g.ind != 0)
            close(g.ind);
        return;
    }

    /* create output file out, descriptor outd */
    if (path == NULL || g.pipeout) {
        /* write to stdout */
        g.outf = MALLOC(strlen("<stdout>") + 1);
        if (g.outf == NULL)
            bail("not enough memory", "");
        strcpy(g.outf, "<stdout>");
        g.outd = 1;
        if (!g.decode && !g.force && isatty(g.outd))
            bail("trying to write compressed data to a terminal",
                  " (use -f to force)");
     }
     else {
        char *to = g.inf, *sufx = "";
        size_t pre = 0;

        /* select parts of the output file name */
        if (g.decode) {
            /* for -dN or -dNT, use the path from the input file and the name
               from the header, stripping any path in the header name */
            if ((g.headis & 1) != 0 && g.hname != NULL) {
                pre = justname(g.inf) - g.inf;
                to = justname(g.hname);
                len = strlen(to);
            }
            /* for -d or -dNn, replace abbreviated suffixes */
            else if (strcmp(to + len, ".tgz") == 0)
                sufx = ".tar";
         }
        else
            /* add appropriate suffix when compressing */
            sufx = g.sufx;
 
         /* create output file and open to write */
        g.outf = MALLOC(pre + len + strlen(sufx) + 1);
         if (g.outf == NULL)
             bail("not enough memory", "");
        memcpy(g.outf, g.inf, pre);
        memcpy(g.outf + pre, to, len);
        strcpy(g.outf + pre + len, sufx);
         g.outd = open(g.outf, O_CREAT | O_TRUNC | O_WRONLY |
                              (g.force ? 0 : O_EXCL), 0600);
 
         /* if exists and not -f, give user a chance to overwrite */
         if (g.outd < 0 && errno == EEXIST && isatty(0) && g.verbosity) {
            int ch, reply;

            fprintf(stderr, "%s exists -- overwrite (y/n)? ", g.outf);
            fflush(stderr);
            reply = -1;
            do {
                ch = getchar();
                if (reply < 0 && ch != ' ' && ch != '\t')
                    reply = ch == 'y' || ch == 'Y' ? 1 : 0;
            } while (ch != EOF && ch != '\n' && ch != '\r');
            if (reply == 1)
                g.outd = open(g.outf, O_CREAT | O_TRUNC | O_WRONLY,
                              0600);
        }

        /* if exists and no overwrite, report and go on to next */
        if (g.outd < 0 && errno == EEXIST) {
            complain("%s exists -- skipping", g.outf);
            RELEASE(g.outf);
            RELEASE(g.hname);
            if (g.ind != 0)
                close(g.ind);
            return;
        }

        /* if some other error, give up */
        if (g.outd < 0)
            bail("write error on ", g.outf);
    }
    SET_BINARY_MODE(g.outd);
    RELEASE(g.hname);

    /* process ind to outd */
    if (g.verbosity > 1)
        fprintf(stderr, "%s to %s ", g.inf, g.outf);
    if (g.decode) {
        if (method == 8)
            infchk();
        else if (method == 257)
            unlzw();
        else
            cat();
    }
#ifndef NOTHREAD
    else if (g.procs > 1)
        parallel_compress();
#endif
    else
        single_compress(0);
    if (g.verbosity > 1) {
        putc('\n', stderr);
        fflush(stderr);
    }

    /* finish up, copy attributes, set times, delete original */
    if (g.ind != 0)
        close(g.ind);
    if (g.outd != 1) {
        if (close(g.outd))
            bail("write error on ", g.outf);
        g.outd = -1;            /* now prevent deletion on interrupt */
        if (g.ind != 0) {
            copymeta(g.inf, g.outf);
            if (!g.keep)
                unlink(g.inf);
        }
        if (g.decode && (g.headis & 2) != 0 && g.stamp)
            touch(g.outf, g.stamp);
    }
    RELEASE(g.outf);
}
