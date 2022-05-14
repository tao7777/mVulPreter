static int run_post_create(const char *dirname)
{
    /* If doesn't start with "g_settings_dump_location/"... */
    if (!dir_is_in_dump_location(dirname))
    {
        /* Then refuse to operate on it (someone is attacking us??) */
         error_msg("Bad problem directory name '%s', should start with: '%s'", dirname, g_settings_dump_location);
         return 400; /* Bad Request */
     }
    if (g_settings_privatereports)
    {
        struct stat statbuf;
        if (lstat(dirname, &statbuf) != 0 || !S_ISDIR(statbuf.st_mode))
        {
            error_msg("Path '%s' isn't directory", dirname);
            return 404; /* Not Found */
        }
        /* Get ABRT's group gid */
        struct group *gr = getgrnam("abrt");
        if (!gr)
        {
            error_msg("Group 'abrt' does not exist");
            return 500;
        }
        if (statbuf.st_uid != 0 || !(statbuf.st_gid == 0 || statbuf.st_gid == gr->gr_gid) || statbuf.st_mode & 07)
        {
            error_msg("Problem directory '%s' isn't owned by root:abrt or others are not restricted from access", dirname);
            return 403;
        }
        struct dump_dir *dd = dd_opendir(dirname, DD_OPEN_READONLY);
        const bool complete = dd && problem_dump_dir_is_complete(dd);
        dd_close(dd);
        if (complete)
        {
            error_msg("Problem directory '%s' has already been processed", dirname);
            return 403;
        }
    }
    else if (!dump_dir_accessible_by_uid(dirname, client_uid))
     {
         if (errno == ENOTDIR)
         {
            error_msg("Path '%s' isn't problem directory", dirname);
            return 404; /* Not Found */
        }
        error_msg("Problem directory '%s' can't be accessed by user with uid %ld", dirname, (long)client_uid);
        return 403; /* Forbidden */
    }

    int child_stdout_fd;
    int child_pid = spawn_event_handler_child(dirname, "post-create", &child_stdout_fd);

    char *dup_of_dir = NULL;
    struct strbuf *cmd_output = strbuf_new();

    bool child_is_post_create = 1; /* else it is a notify child */

 read_child_output:

    /* Read streamed data and split lines */
    for (;;)
    {
        char buf[250]; /* usually we get one line, no need to have big buf */
        errno = 0;
        int r = safe_read(child_stdout_fd, buf, sizeof(buf) - 1);
        if (r <= 0)
            break;
        buf[r] = '\0';

        /* split lines in the current buffer */
        char *raw = buf;
        char *newline;
        while ((newline = strchr(raw, '\n')) != NULL)
        {
            *newline = '\0';
            strbuf_append_str(cmd_output, raw);
            char *msg = cmd_output->buf;

            /* Hmm, DUP_OF_DIR: ends up in syslog. move log() into 'else'? */
            log("%s", msg);

            if (child_is_post_create
             && prefixcmp(msg, "DUP_OF_DIR: ") == 0
            ) {
                free(dup_of_dir);
                dup_of_dir = xstrdup(msg + strlen("DUP_OF_DIR: "));
            }

            strbuf_clear(cmd_output);
            /* jump to next line */
            raw = newline + 1;
        }

        /* beginning of next line. the line continues by next read */
        strbuf_append_str(cmd_output, raw);
    }

    /* EOF/error */

    /* Wait for child to actually exit, collect status */
    int status = 0;
    if (safe_waitpid(child_pid, &status, 0) <= 0)
    /* should not happen */
        perror_msg("waitpid(%d)", child_pid);

    /* If it was a "notify[-dup]" event, then we're done */
    if (!child_is_post_create)
        goto ret;

    /* exit 0 means "this is a good, non-dup dir" */
    /* exit with 1 + "DUP_OF_DIR: dir" string => dup */
    if (status != 0)
    {
        if (WIFSIGNALED(status))
        {
            log("'post-create' on '%s' killed by signal %d",
                            dirname, WTERMSIG(status));
            goto delete_bad_dir;
        }
        /* else: it is WIFEXITED(status) */
        if (!dup_of_dir)
        {
            log("'post-create' on '%s' exited with %d",
                            dirname, WEXITSTATUS(status));
            goto delete_bad_dir;
        }
    }

    const char *work_dir = (dup_of_dir ? dup_of_dir : dirname);

    /* Load problem_data (from the *first dir* if this one is a dup) */
    struct dump_dir *dd = dd_opendir(work_dir, /*flags:*/ 0);
    if (!dd)
        /* dd_opendir already emitted error msg */
        goto delete_bad_dir;

    /* Update count */
    char *count_str = dd_load_text_ext(dd, FILENAME_COUNT, DD_FAIL_QUIETLY_ENOENT);
    unsigned long count = strtoul(count_str, NULL, 10);

    /* Don't increase crash count if we are working with newly uploaded
     * directory (remote crash) which already has its crash count set.
     */
    if ((status != 0 && dup_of_dir) || count == 0)
    {
        count++;
        char new_count_str[sizeof(long)*3 + 2];
        sprintf(new_count_str, "%lu", count);
        dd_save_text(dd, FILENAME_COUNT, new_count_str);

        /* This condition can be simplified to either
         * (status * != 0 && * dup_of_dir) or (count == 1). But the
         * chosen form is much more reliable and safe. We must not call
         * dd_opendir() to locked dd otherwise we go into a deadlock.
         */
        if (strcmp(dd->dd_dirname, dirname) != 0)
        {
            /* Update the last occurrence file by the time file of the new problem */
            struct dump_dir *new_dd = dd_opendir(dirname, DD_OPEN_READONLY);
            char *last_ocr = NULL;
            if (new_dd)
            {
                /* TIME must exists in a valid dump directory but we don't want to die
                 * due to broken duplicated dump directory */
                last_ocr = dd_load_text_ext(new_dd, FILENAME_TIME,
                            DD_LOAD_TEXT_RETURN_NULL_ON_FAILURE | DD_FAIL_QUIETLY_ENOENT);
                dd_close(new_dd);
            }
            else
            {   /* dd_opendir() already produced a message with good information about failure */
                error_msg("Can't read the last occurrence file from the new dump directory.");
            }

            if (!last_ocr)
            {   /* the new dump directory may lie in the dump location for some time */
                log("Using current time for the last occurrence file which may be incorrect.");
                time_t t = time(NULL);
                last_ocr = xasprintf("%lu", (long)t);
            }

            dd_save_text(dd, FILENAME_LAST_OCCURRENCE, last_ocr);

            free(last_ocr);
        }
    }

    /* Reset mode/uig/gid to correct values for all files created by event run */
    dd_sanitize_mode_and_owner(dd);

    dd_close(dd);

    if (!dup_of_dir)
        log_notice("New problem directory %s, processing", work_dir);
    else
    {
        log_warning("Deleting problem directory %s (dup of %s)",
                    strrchr(dirname, '/') + 1,
                    strrchr(dup_of_dir, '/') + 1);
        delete_dump_dir(dirname);
    }

    /* Run "notify[-dup]" event */
    int fd;
    child_pid = spawn_event_handler_child(
                work_dir,
                (dup_of_dir ? "notify-dup" : "notify"),
                &fd
    );
    xmove_fd(fd, child_stdout_fd);
    child_is_post_create = 0;
    strbuf_clear(cmd_output);
    free(dup_of_dir);
    dup_of_dir = NULL;
    goto read_child_output;

 delete_bad_dir:
    log_warning("Deleting problem directory '%s'", dirname);
    delete_dump_dir(dirname);

 ret:
    strbuf_free(cmd_output);
    free(dup_of_dir);
    close(child_stdout_fd);
    return 0;
}
