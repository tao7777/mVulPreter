ProcDbeGetVisualInfo(ClientPtr client)
{
    REQUEST(xDbeGetVisualInfoReq);
    DbeScreenPrivPtr pDbeScreenPriv;
    xDbeGetVisualInfoReply rep;
    Drawable *drawables;
    DrawablePtr *pDrawables = NULL;
    register int i, j, rc;
    register int count;         /* number of visual infos in reply */
    register int length;        /* length of reply */
    ScreenPtr pScreen;
     XdbeScreenVisualInfo *pScrVisInfo;
 
     REQUEST_AT_LEAST_SIZE(xDbeGetVisualInfoReq);
    if (stuff->n > UINT32_MAX / sizeof(CARD32))
        return BadLength;
    REQUEST_FIXED_SIZE(xDbeGetVisualInfoReq, stuff->n * sizeof(CARD32));
 
     if (stuff->n > UINT32_MAX / sizeof(DrawablePtr))
         return BadAlloc;
            return BadAlloc;
        }
