static void ParseCommon(map_string_t *settings, const char *conf_filename)
{
    const char *value;

    value = get_map_string_item_or_NULL(settings, "WatchCrashdumpArchiveDir");
    if (value)
    {
        g_settings_sWatchCrashdumpArchiveDir = xstrdup(value);
        remove_map_string_item(settings, "WatchCrashdumpArchiveDir");
    }

    value = get_map_string_item_or_NULL(settings, "MaxCrashReportsSize");
    if (value)
    {
        char *end;
        errno = 0;
        unsigned long ul = strtoul(value, &end, 10);
        if (errno || end == value || *end != '\0' || ul > INT_MAX)
            error_msg("Error parsing %s setting: '%s'", "MaxCrashReportsSize", value);
        else
            g_settings_nMaxCrashReportsSize = ul;
        remove_map_string_item(settings, "MaxCrashReportsSize");
    }

    value = get_map_string_item_or_NULL(settings, "DumpLocation");
    if (value)
    {
        g_settings_dump_location = xstrdup(value);
        remove_map_string_item(settings, "DumpLocation");
    }
    else
        g_settings_dump_location = xstrdup(DEFAULT_DUMP_LOCATION);

    value = get_map_string_item_or_NULL(settings, "DeleteUploaded");
    if (value)
    {
        g_settings_delete_uploaded = string_to_bool(value);
        remove_map_string_item(settings, "DeleteUploaded");
    }

    value = get_map_string_item_or_NULL(settings, "AutoreportingEnabled");
    if (value)
    {
        g_settings_autoreporting = string_to_bool(value);
        remove_map_string_item(settings, "AutoreportingEnabled");
    }

    value = get_map_string_item_or_NULL(settings, "AutoreportingEvent");
    if (value)
    {
        g_settings_autoreporting_event = xstrdup(value);
        remove_map_string_item(settings, "AutoreportingEvent");
    }
    else
        g_settings_autoreporting_event = xstrdup("report_uReport");

    value = get_map_string_item_or_NULL(settings, "ShortenedReporting");
    if (value)
    {
        g_settings_shortenedreporting = string_to_bool(value);
        remove_map_string_item(settings, "ShortenedReporting");
    }
     else
         g_settings_shortenedreporting = 0;
 
    value = get_map_string_item_or_NULL(settings, "PrivateReports");
    if (value)
    {
        g_settings_privatereports = string_to_bool(value);
        remove_map_string_item(settings, "PrivateReports");
    }

     GHashTableIter iter;
     const char *name;
     /*char *value; - already declared */
    init_map_string_iter(&iter, settings);
    while (next_map_string_iter(&iter, &name, &value))
    {
        error_msg("Unrecognized variable '%s' in '%s'", name, conf_filename);
    }
}
