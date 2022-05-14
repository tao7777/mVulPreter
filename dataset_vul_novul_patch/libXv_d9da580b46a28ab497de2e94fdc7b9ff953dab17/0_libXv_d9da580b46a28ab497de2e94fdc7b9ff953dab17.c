XvQueryAdaptors(
    Display *dpy,
    Window window,
    unsigned int *p_nAdaptors,
    XvAdaptorInfo **p_pAdaptors)
{
    XExtDisplayInfo *info = xv_find_display(dpy);
    xvQueryAdaptorsReq *req;
    xvQueryAdaptorsReply rep;
     size_t size;
     unsigned int ii, jj;
     char *name;
    char *end;
     XvAdaptorInfo *pas = NULL, *pa;
     XvFormat *pfs, *pf;
     char *buffer = NULL;
        char *buffer;
        char *string;
        xvAdaptorInfo *pa;
        xvFormat *pf;
    } u;
