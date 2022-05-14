 logger_get_mask_expanded (struct t_gui_buffer *buffer, const char *mask)
 {
    char *mask2, *mask3, *mask4, *mask5, *mask6, *mask7;
     const char *dir_separator;
     int length;
     time_t seconds;
     struct tm *date_tmp;
 
     mask2 = NULL;
    mask3 = NULL;
    mask4 = NULL;
    mask5 = NULL;
    mask6 = NULL;
    mask7 = NULL;
 
     dir_separator = weechat_info_get ("dir_separator", "");
     if (!dir_separator)
         return NULL;
 
    /* replace date/time specifiers in mask */
    length = strlen (mask) + 256 + 1;
    mask2 = malloc (length);
    if (!mask2)
        goto end;
    seconds = time (NULL);
    date_tmp = localtime (&seconds);
    mask2[0] = '\0';
    if (strftime (mask2, length - 1, mask, date_tmp) == 0)
        mask2[0] = '\0';

     /*
      * we first replace directory separator (commonly '/') by \01 because
      * buffer mask can contain this char, and will be replaced by replacement
      * char ('_' by default)
      */
    mask3 = weechat_string_replace (mask2, dir_separator, "\01");
    if (!mask3)
         goto end;
 
    mask4 = weechat_buffer_string_replace_local_var (buffer, mask3);
    if (!mask4)
         goto end;
 
    mask5 = weechat_string_replace (mask4,
                                    dir_separator,
                                    weechat_config_string (logger_config_file_replacement_char));
    if (!mask5)
         goto end;
 
 #ifdef __CYGWIN__
    mask6 = weechat_string_replace (mask5, "\\",
                                    weechat_config_string (logger_config_file_replacement_char));
 #else
    mask6 = strdup (mask5);
 #endif /* __CYGWIN__ */
    if (!mask6)
         goto end;
 
     /* restore directory separator */
    mask7 = weechat_string_replace (mask6,
                                    "\01", dir_separator);
    if (!mask7)
         goto end;
 
     /* convert to lower case? */
     if (weechat_config_boolean (logger_config_file_name_lower_case))
        weechat_string_tolower (mask7);
 
     if (weechat_logger_plugin->debug)
     {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  "%s: buffer = \"%s\", mask = \"%s\", "
                                   "decoded mask = \"%s\"",
                                   LOGGER_PLUGIN_NAME,
                                   weechat_buffer_get_string (buffer, "name"),
                                  mask, mask7);
     }
 
 end:
     if (mask2)
         free (mask2);
    if (mask3)
        free (mask3);
    if (mask4)
        free (mask4);
    if (mask5)
        free (mask5);
    if (mask6)
        free (mask6);

    return mask7;
 }
