static int create_problem_dir(GHashTable *problem_info, unsigned pid)
{
    /* Exit if free space is less than 1/4 of MaxCrashReportsSize */
    if (g_settings_nMaxCrashReportsSize > 0)
    {
        if (low_free_space(g_settings_nMaxCrashReportsSize, g_settings_dump_location))
            exit(1);
    }

    /* Create temp directory with the problem data.
     * This directory is renamed to final directory name after
     * all files have been stored into it.
     */

    gchar *dir_basename = g_hash_table_lookup(problem_info, "basename");
    if (!dir_basename)
        dir_basename = g_hash_table_lookup(problem_info, FILENAME_TYPE);

    char *path = xasprintf("%s/%s-%s-%u.new",
                           g_settings_dump_location,
                           dir_basename,
                           iso_date_string(NULL),
                           pid);

    /* This item is useless, don't save it */
    g_hash_table_remove(problem_info, "basename");

     /* No need to check the path length, as all variables used are limited,
      * and dd_create() fails if the path is too long.
      */
    struct dump_dir *dd = dd_create(path, g_settings_privatereports ? 0 : client_uid, DEFAULT_DUMP_DIR_MODE);
     if (!dd)
     {
         error_msg_and_die("Error creating problem directory '%s'", path);
    }

    dd_create_basic_files(dd, client_uid, NULL);
    dd_save_text(dd, FILENAME_ABRT_VERSION, VERSION);

    gpointer gpkey = g_hash_table_lookup(problem_info, FILENAME_CMDLINE);
    if (!gpkey)
    {
        /* Obtain and save the command line. */
        char *cmdline = get_cmdline(pid);
        if (cmdline)
        {
            dd_save_text(dd, FILENAME_CMDLINE, cmdline);
            free(cmdline);
        }
    }

    /* Store id of the user whose application crashed. */
    char uid_str[sizeof(long) * 3 + 2];
    sprintf(uid_str, "%lu", (long)client_uid);
    dd_save_text(dd, FILENAME_UID, uid_str);

    GHashTableIter iter;
    gpointer gpvalue;
    g_hash_table_iter_init(&iter, problem_info);
    while (g_hash_table_iter_next(&iter, &gpkey, &gpvalue))
    {
        dd_save_text(dd, (gchar *) gpkey, (gchar *) gpvalue);
    }

    dd_close(dd);

    /* Not needing it anymore */
    g_hash_table_destroy(problem_info);

    /* Move the completely created problem directory
     * to final directory.
     */
    char *newpath = xstrndup(path, strlen(path) - strlen(".new"));
    if (rename(path, newpath) == 0)
        strcpy(path, newpath);
    free(newpath);

    log_notice("Saved problem directory of pid %u to '%s'", pid, path);

    /* We let the peer know that problem dir was created successfully
     * _before_ we run potentially long-running post-create.
     */
    printf("HTTP/1.1 201 Created\r\n\r\n");
    fflush(NULL);
    close(STDOUT_FILENO);
    xdup2(STDERR_FILENO, STDOUT_FILENO); /* paranoia: don't leave stdout fd closed */

    /* Trim old problem directories if necessary */
    if (g_settings_nMaxCrashReportsSize > 0)
    {
        trim_problem_dirs(g_settings_dump_location, g_settings_nMaxCrashReportsSize * (double)(1024*1024), path);
    }

    run_post_create(path);

    /* free(path); */
    exit(0);
}
