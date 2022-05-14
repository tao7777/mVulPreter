char* problem_data_save(problem_data_t *pd)
 {
     load_abrt_conf();
 
    struct dump_dir *dd = NULL;

    if (g_settings_privatereports)
        dd = create_dump_dir_from_problem_data_ext(pd, g_settings_dump_location, 0);
    else
        dd = create_dump_dir_from_problem_data(pd, g_settings_dump_location);
 
     char *problem_id = NULL;
     if (dd)
    {
        problem_id = xstrdup(dd->dd_dirname);
        dd_close(dd);
    }

    log_info("problem id: '%s'", problem_id);
    return problem_id;
}
