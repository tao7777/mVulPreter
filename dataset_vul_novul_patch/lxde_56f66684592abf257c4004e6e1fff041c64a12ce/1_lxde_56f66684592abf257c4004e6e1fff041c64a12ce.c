static void get_socket_name( char* buf, int len )
{
    char* dpy = g_strdup(g_getenv("DISPLAY"));
    if(dpy && *dpy)
    {
        char* p = strchr(dpy, ':');
        for(++p; *p && *p != '.' && *p != '\n';)
            ++p;
         if(*p)
             *p = '\0';
     }
     g_snprintf( buf, len, "%s/.menu-cached-%s-%s", g_get_tmp_dir(),
                 dpy ? dpy : ":0", g_get_user_name() );
     g_free(dpy);
 }
