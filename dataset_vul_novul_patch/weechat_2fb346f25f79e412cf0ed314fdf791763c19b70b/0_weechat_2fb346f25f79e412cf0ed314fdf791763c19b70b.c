irc_ctcp_dcc_filename_without_quotes (const char *filename)
{
     int length;
 
     length = strlen (filename);
    if (length > 1)
     {
         if ((filename[0] == '\"') && (filename[length - 1] == '\"'))
             return weechat_strndup (filename + 1, length - 2);
    }
    return strdup (filename);
}
