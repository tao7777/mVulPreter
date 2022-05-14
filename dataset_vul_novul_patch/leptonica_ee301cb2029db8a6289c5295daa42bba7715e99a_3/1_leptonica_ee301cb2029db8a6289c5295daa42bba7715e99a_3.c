 gplotMakeOutput(GPLOT  *gplot)
 {
char     buf[L_BUF_SIZE];
 char    *cmdname;
 l_int32  ignore;
 
    PROCNAME("gplotMakeOutput");

    if (!gplot)
        return ERROR_INT("gplot not defined", procName, 1);

    gplotGenCommandFile(gplot);
    gplotGenDataFiles(gplot);
     cmdname = genPathname(gplot->cmdname, NULL);
 
 #ifndef _WIN32
    snprintf(buf, L_BUF_SIZE, "gnuplot %s", cmdname);
 #else
    snprintf(buf, L_BUF_SIZE, "wgnuplot %s", cmdname);
 #endif  /* _WIN32 */
 
 #ifndef OS_IOS /* iOS 11 does not support system() */
    ignore = system(buf);  /* gnuplot || wgnuplot */
#endif /* !OS_IOS */

    LEPT_FREE(cmdname);
    return 0;
}
