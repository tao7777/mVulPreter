ProcRenderAddGlyphs (ClientPtr client)
{
    GlyphSetPtr     glyphSet;
    REQUEST(xRenderAddGlyphsReq);
    GlyphNewRec	    glyphsLocal[NLOCALGLYPH];
    GlyphNewPtr	    glyphsBase, glyphs, glyph_new;
    int		    remain, nglyphs;
    CARD32	    *gids;
    xGlyphInfo	    *gi;
    CARD8	    *bits;
    unsigned int    size;
    int		    err;
    int		    i, screen;
    PicturePtr	    pSrc = NULL, pDst = NULL;
    PixmapPtr	    pSrcPix = NULL, pDstPix = NULL;
    CARD32	    component_alpha;

    REQUEST_AT_LEAST_SIZE(xRenderAddGlyphsReq);
    err = dixLookupResourceByType((pointer *)&glyphSet, stuff->glyphset, GlyphSetType,
			    client, DixAddAccess);
    if (err != Success)
    {
	client->errorValue = stuff->glyphset;
	return err;
    }

    err = BadAlloc;
    nglyphs = stuff->nglyphs;
    if (nglyphs > UINT32_MAX / sizeof(GlyphNewRec))
	    return BadAlloc;

    component_alpha = NeedsComponent (glyphSet->format->format);

    if (nglyphs <= NLOCALGLYPH) {
	memset (glyphsLocal, 0, sizeof (glyphsLocal));
	glyphsBase = glyphsLocal;
    }
    else
    {
	glyphsBase = (GlyphNewPtr)calloc(nglyphs, sizeof (GlyphNewRec));
	if (!glyphsBase)
	    return BadAlloc;
    }

    remain = (client->req_len << 2) - sizeof (xRenderAddGlyphsReq);

    glyphs = glyphsBase;

    gids = (CARD32 *) (stuff + 1);
     gi = (xGlyphInfo *) (gids + nglyphs);
     bits = (CARD8 *) (gi + nglyphs);
     remain -= (sizeof (CARD32) + sizeof (xGlyphInfo)) * nglyphs;

    /* protect against bad nglyphs */
    if (gi < stuff || gi > ((CARD32 *)stuff + client->req_len) ||
        bits < stuff || bits > ((CARD32 *)stuff + client->req_len)) {
        err = BadLength;
        goto bail;
    }

     for (i = 0; i < nglyphs; i++)
     {
 	size_t padded_width;
	size = gi[i].height * padded_width;
	if (remain < size)
	    break;

	err = HashGlyph (&gi[i], bits, size, glyph_new->sha1);
	if (err)
	    goto bail;

	glyph_new->glyph = FindGlyphByHash (glyph_new->sha1,
					    glyphSet->fdepth);

	if (glyph_new->glyph && glyph_new->glyph != DeletedGlyph)
	{
	    glyph_new->found = TRUE;
	}
	else
	{
	    GlyphPtr glyph;

	    glyph_new->found = FALSE;
	    glyph_new->glyph = glyph = AllocateGlyph (&gi[i], glyphSet->fdepth);
	    if (! glyph)
	    {
		err = BadAlloc;
		goto bail;
	    }

	    for (screen = 0; screen < screenInfo.numScreens; screen++)
	    {
		int	    width = gi[i].width;
		int	    height = gi[i].height;
		int	    depth = glyphSet->format->depth;
		ScreenPtr   pScreen;
		int	    error;

		/* Skip work if it's invisibly small anyway */
		if (!width || !height)
		    break;

		pScreen = screenInfo.screens[screen];
		pSrcPix = GetScratchPixmapHeader (pScreen,
						  width, height,
						  depth, depth,
						  -1, bits);
		if (! pSrcPix)
		{
		    err = BadAlloc;
		    goto bail;
		}

		pSrc = CreatePicture (0, &pSrcPix->drawable,
				      glyphSet->format, 0, NULL,
				      serverClient, &error);
		if (! pSrc)
		{
		    err = BadAlloc;
		    goto bail;
		}

		pDstPix = (pScreen->CreatePixmap) (pScreen,
						   width, height, depth,
						   CREATE_PIXMAP_USAGE_GLYPH_PICTURE);

		if (!pDstPix)
		{
		    err = BadAlloc;
		    goto bail;
		}

		GlyphPicture (glyph)[screen] = pDst =
			CreatePicture (0, &pDstPix->drawable,
				       glyphSet->format,
				       CPComponentAlpha, &component_alpha,
				       serverClient, &error);

		/* The picture takes a reference to the pixmap, so we
		   drop ours. */
		(pScreen->DestroyPixmap) (pDstPix);
		pDstPix = NULL;

		if (! pDst)
		{
		    err = BadAlloc;
		    goto bail;
		}

		CompositePicture (PictOpSrc,
				  pSrc,
				  None,
				  pDst,
				  0, 0,
				  0, 0,
				  0, 0,
				  width, height);

		FreePicture ((pointer) pSrc, 0);
		pSrc = NULL;
		FreeScratchPixmapHeader (pSrcPix);
		pSrcPix = NULL;
	    }

	    memcpy (glyph_new->glyph->sha1, glyph_new->sha1, 20);
	}

	glyph_new->id = gids[i];
	
	if (size & 3)
	    size += 4 - (size & 3);
	bits += size;
	remain -= size;
    }
    if (remain || i < nglyphs)
    {
	err = BadLength;
	goto bail;
    }
    if (!ResizeGlyphSet (glyphSet, nglyphs))
    {
	err = BadAlloc;
	goto bail;
    }
    for (i = 0; i < nglyphs; i++)
	AddGlyph (glyphSet, glyphs[i].glyph, glyphs[i].id);

    if (glyphsBase != glyphsLocal)
	free(glyphsBase);
    return Success;
bail:
    if (pSrc)
	FreePicture ((pointer) pSrc, 0);
    if (pSrcPix)
	FreeScratchPixmapHeader (pSrcPix);
    for (i = 0; i < nglyphs; i++)
	if (glyphs[i].glyph && ! glyphs[i].found)
	    free(glyphs[i].glyph);
    if (glyphsBase != glyphsLocal)
	free(glyphsBase);
    return err;
}
