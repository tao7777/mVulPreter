static unsigned create_oops_dump_dirs(GList *oops_list, unsigned oops_cnt)
{
    unsigned countdown = MAX_DUMPED_DD_COUNT; /* do not report hundreds of oopses */

    log_notice("Saving %u oopses as problem dirs", oops_cnt >= countdown ? countdown : oops_cnt);

    char *cmdline_str = xmalloc_fopen_fgetline_fclose("/proc/cmdline");
    char *fips_enabled = xmalloc_fopen_fgetline_fclose("/proc/sys/crypto/fips_enabled");
    char *proc_modules = xmalloc_open_read_close("/proc/modules", /*maxsize:*/ NULL);
    char *suspend_stats = xmalloc_open_read_close("/sys/kernel/debug/suspend_stats", /*maxsize:*/ NULL);

    time_t t = time(NULL);
    const char *iso_date = iso_date_string(&t);
    /* dump should be readable by all if we're run with -x */
    uid_t my_euid = (uid_t)-1L;
    mode_t mode = DEFAULT_DUMP_DIR_MODE | S_IROTH;
    /* and readable only for the owner otherwise */
    if (!world_readable_dump)
    {
         mode = DEFAULT_DUMP_DIR_MODE;
         my_euid = geteuid();
     }
 
     pid_t my_pid = getpid();
     unsigned idx = 0;
    unsigned errors = 0;
    while (idx < oops_cnt)
    {
        char base[sizeof("oops-YYYY-MM-DD-hh:mm:ss-%lu-%lu") + 2 * sizeof(long)*3];
        sprintf(base, "oops-%s-%lu-%lu", iso_date, (long)my_pid, (long)idx);
        char *path = concat_path_file(debug_dumps_dir, base);

        struct dump_dir *dd = dd_create(path, /*uid:*/ my_euid, mode);
        if (dd)
        {
            dd_create_basic_files(dd, /*uid:*/ my_euid, NULL);
            save_oops_data_in_dump_dir(dd, (char*)g_list_nth_data(oops_list, idx++), proc_modules);
            dd_save_text(dd, FILENAME_ABRT_VERSION, VERSION);
            dd_save_text(dd, FILENAME_ANALYZER, "Kerneloops");
            dd_save_text(dd, FILENAME_TYPE, "Kerneloops");
            if (cmdline_str)
                dd_save_text(dd, FILENAME_CMDLINE, cmdline_str);
            if (proc_modules)
                dd_save_text(dd, "proc_modules", proc_modules);
            if (fips_enabled && strcmp(fips_enabled, "0") != 0)
                dd_save_text(dd, "fips_enabled", fips_enabled);
            if (suspend_stats)
                dd_save_text(dd, "suspend_stats", suspend_stats);
            dd_close(dd);
            notify_new_path(path);
        }
        else
            errors++;

        free(path);

        if (--countdown == 0)
            break;

        if (dd && throttle_dd_creation)
            sleep(1);
    }

    free(cmdline_str);
    free(proc_modules);
    free(fips_enabled);
    free(suspend_stats);

    return errors;
}
