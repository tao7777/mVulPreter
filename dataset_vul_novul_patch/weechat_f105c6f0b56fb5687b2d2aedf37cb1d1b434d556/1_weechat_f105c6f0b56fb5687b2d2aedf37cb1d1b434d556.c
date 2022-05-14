 logger_get_mask_expanded (struct t_gui_buffer *buffer, const char *mask)
 {
    char *mask2, *mask_decoded, *mask_decoded2, *mask_decoded3, *mask_decoded4;
    char *mask_decoded5;
     const char *dir_separator;
     int length;
     time_t seconds;
     struct tm *date_tmp;
 
     mask2 = NULL;
    mask_decoded = NULL;
    mask_decoded2 = NULL;
    mask_decoded3 = NULL;
    mask_decoded4 = NULL;
    mask_decoded5 = NULL;
 
     dir_separator = weechat_info_get ("dir_separator", "");
     if (!dir_separator)
         return NULL;
 
     /*
      * we first replace directory separator (commonly '/') by \01 because
      * buffer mask can contain this char, and will be replaced by replacement
      * char ('_' by default)
      */
    mask2 = weechat_string_replace (mask, dir_separator, "\01");
    if (!mask2)
         goto end;
 
    mask_decoded = weechat_buffer_string_replace_local_var (buffer, mask2);
    if (!mask_decoded)
         goto end;
 
    mask_decoded2 = weechat_string_replace (mask_decoded,
                                            dir_separator,
                                            weechat_config_string (logger_config_file_replacement_char));
    if (!mask_decoded2)
         goto end;
 
 #ifdef __CYGWIN__
    mask_decoded3 = weechat_string_replace (mask_decoded2, "\\",
                                            weechat_config_string (logger_config_file_replacement_char));
 #else
    mask_decoded3 = strdup (mask_decoded2);
 #endif /* __CYGWIN__ */
    if (!mask_decoded3)
         goto end;
 
     /* restore directory separator */
    mask_decoded4 = weechat_string_replace (mask_decoded3,
                                            "\01", dir_separator);
    if (!mask_decoded4)
        goto end;
    /* replace date/time specifiers in mask */
    length = strlen (mask_decoded4) + 256 + 1;
    mask_decoded5 = malloc (length);
    if (!mask_decoded5)
         goto end;
    seconds = time (NULL);
    date_tmp = localtime (&seconds);
    mask_decoded5[0] = '\0';
    strftime (mask_decoded5, length - 1, mask_decoded4, date_tmp);
 
     /* convert to lower case? */
     if (weechat_config_boolean (logger_config_file_name_lower_case))
        weechat_string_tolower (mask_decoded5);
 
     if (weechat_logger_plugin->debug)
     {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  "%s: buffer = \"%s\", mask = \"%s\", "
                                   "decoded mask = \"%s\"",
                                   LOGGER_PLUGIN_NAME,
                                   weechat_buffer_get_string (buffer, "name"),
                                  mask, mask_decoded5);
     }
 
 end:
     if (mask2)
         free (mask2);
    if (mask_decoded)
        free (mask_decoded);
    if (mask_decoded2)
        free (mask_decoded2);
    if (mask_decoded3)
        free (mask_decoded3);
    if (mask_decoded4)
        free (mask_decoded4);
    return mask_decoded5;
 }
