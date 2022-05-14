static void get_socket_name(SingleInstData* data, char* buf, int len)
{
    const char* dpy = g_getenv("DISPLAY");
    char* host = NULL;
    int dpynum;
    if(dpy)
    {
        const char* p = strrchr(dpy, ':');
        host = g_strndup(dpy, (p - dpy));
        dpynum = atoi(p + 1);
     }
     else
         dpynum = 0;
#if GLIB_CHECK_VERSION(2, 28, 0)
    g_snprintf(buf, len, "%s/%s-socket-%s-%d", g_get_user_runtime_dir(),
               data->prog_name, host ? host : "", dpynum);
#else
     g_snprintf(buf, len, "%s/.%s-socket-%s-%d-%s",
                 g_get_tmp_dir(),
                 data->prog_name,
                 host ? host : "",
                 dpynum,
                 g_get_user_name());
#endif
 }
