bdfReadCharacters(FontFilePtr file, FontPtr pFont, bdfFileState *pState,
		  int bit, int byte, int glyph, int scan)
{
    unsigned char *line;
    register CharInfoPtr ci;
    int         i,
                ndx,
                nchars,
                nignored;
    unsigned int char_row, char_col;
    int         numEncodedGlyphs = 0;
    CharInfoPtr *bdfEncoding[256];
    BitmapFontPtr  bitmapFont;
    BitmapExtraPtr bitmapExtra;
    CARD32     *bitmapsSizes;
    unsigned char        lineBuf[BDFLINELEN];
    int         nencoding;

    bitmapFont = (BitmapFontPtr) pFont->fontPrivate;
    bitmapExtra = (BitmapExtraPtr) bitmapFont->bitmapExtra;

    if (bitmapExtra) {
	bitmapsSizes = bitmapExtra->bitmapsSizes;
	for (i = 0; i < GLYPHPADOPTIONS; i++)
	    bitmapsSizes[i] = 0;
    } else
	bitmapsSizes = NULL;

    bzero(bdfEncoding, sizeof(bdfEncoding));
    bitmapFont->metrics = NULL;
    ndx = 0;

    line = bdfGetLine(file, lineBuf, BDFLINELEN);

    if ((!line) || (sscanf((char *) line, "CHARS %d", &nchars) != 1)) {
	bdfError("bad 'CHARS' in bdf file\n");
	return (FALSE);
    }
    if (nchars < 1) {
	bdfError("invalid number of CHARS in BDF file\n");
	return (FALSE);
    }
    if (nchars > INT32_MAX / sizeof(CharInfoRec)) {
	bdfError("Couldn't allocate pCI (%d*%d)\n", nchars,
		 (int) sizeof(CharInfoRec));
	goto BAILOUT;
    }
    ci = calloc(nchars, sizeof(CharInfoRec));
    if (!ci) {
	bdfError("Couldn't allocate pCI (%d*%d)\n", nchars,
		 (int) sizeof(CharInfoRec));
	goto BAILOUT;
    }
    bitmapFont->metrics = ci;

    if (bitmapExtra) {
	bitmapExtra->glyphNames = malloc(nchars * sizeof(Atom));
	if (!bitmapExtra->glyphNames) {
	    bdfError("Couldn't allocate glyphNames (%d*%d)\n",
		     nchars, (int) sizeof(Atom));
	    goto BAILOUT;
	}
    }
    if (bitmapExtra) {
	bitmapExtra->sWidths = malloc(nchars * sizeof(int));
	if (!bitmapExtra->sWidths) {
	    bdfError("Couldn't allocate sWidth (%d *%d)\n",
		     nchars, (int) sizeof(int));
	    return FALSE;
	}
    }
    line = bdfGetLine(file, lineBuf, BDFLINELEN);
    pFont->info.firstRow = 256;
    pFont->info.lastRow = 0;
    pFont->info.firstCol = 256;
    pFont->info.lastCol = 0;
    nignored = 0;
    for (ndx = 0; (ndx < nchars) && (line) && (bdfIsPrefix(line, "STARTCHAR"));) {
	int         t;
	int         wx;		/* x component of width */
	int         wy;		/* y component of width */
	int         bw;		/* bounding-box width */
	int         bh;		/* bounding-box height */
	int         bl;		/* bounding-box left */
	int         bb;		/* bounding-box bottom */
	int         enc,
	            enc2;	/* encoding */
	unsigned char *p;	/* temp pointer into line */
 	char        charName[100];
 	int         ignore;
 
	if (sscanf((char *) line, "STARTCHAR %99s", charName) != 1) {
 	    bdfError("bad character name in BDF file\n");
 	    goto BAILOUT;	/* bottom of function, free and return error */
 	}
	if (bitmapExtra)
	    bitmapExtra->glyphNames[ndx] = bdfForceMakeAtom(charName, NULL);

	line = bdfGetLine(file, lineBuf, BDFLINELEN);
	if (!line || (t = sscanf((char *) line, "ENCODING %d %d", &enc, &enc2)) < 1) {
	    bdfError("bad 'ENCODING' in BDF file\n");
	    goto BAILOUT;
	}
	if (enc < -1 || (t == 2 && enc2 < -1)) {
	    bdfError("bad ENCODING value");
	    goto BAILOUT;
	}
	if (t == 2 && enc == -1)
	    enc = enc2;
	ignore = 0;
	if (enc == -1) {
	    if (!bitmapExtra) {
		nignored++;
		ignore = 1;
	    }
	} else if (enc > MAXENCODING) {
	    bdfError("char '%s' has encoding too large (%d)\n",
		     charName, enc);
	} else {
	    char_row = (enc >> 8) & 0xFF;
	    char_col = enc & 0xFF;
	    if (char_row < pFont->info.firstRow)
		pFont->info.firstRow = char_row;
	    if (char_row > pFont->info.lastRow)
		pFont->info.lastRow = char_row;
	    if (char_col < pFont->info.firstCol)
		pFont->info.firstCol = char_col;
	    if (char_col > pFont->info.lastCol)
		pFont->info.lastCol = char_col;
	    if (bdfEncoding[char_row] == (CharInfoPtr *) NULL) {
		bdfEncoding[char_row] = malloc(256 * sizeof(CharInfoPtr));
		if (!bdfEncoding[char_row]) {
		    bdfError("Couldn't allocate row %d of encoding (%d*%d)\n",
			     char_row, INDICES, (int) sizeof(CharInfoPtr));
		    goto BAILOUT;
		}
		for (i = 0; i < 256; i++)
		    bdfEncoding[char_row][i] = (CharInfoPtr) NULL;
	    }
	    if (bdfEncoding[char_row] != NULL) {
		bdfEncoding[char_row][char_col] = ci;
		numEncodedGlyphs++;
	    }
	}

	line = bdfGetLine(file, lineBuf, BDFLINELEN);
	if ((!line) || (sscanf((char *) line, "SWIDTH %d %d", &wx, &wy) != 2)) {
	    bdfError("bad 'SWIDTH'\n");
	    goto BAILOUT;
	}
	if (wy != 0) {
	    bdfError("SWIDTH y value must be zero\n");
	    goto BAILOUT;
	}
	if (bitmapExtra)
	    bitmapExtra->sWidths[ndx] = wx;

/* 5/31/89 (ef) -- we should be able to ditch the character and recover */
/*		from all of these.					*/

	line = bdfGetLine(file, lineBuf, BDFLINELEN);
	if ((!line) || (sscanf((char *) line, "DWIDTH %d %d", &wx, &wy) != 2)) {
	    bdfError("bad 'DWIDTH'\n");
	    goto BAILOUT;
	}
	if (wy != 0) {
	    bdfError("DWIDTH y value must be zero\n");
	    goto BAILOUT;
	}
	line = bdfGetLine(file, lineBuf, BDFLINELEN);
	if ((!line) || (sscanf((char *) line, "BBX %d %d %d %d", &bw, &bh, &bl, &bb) != 4)) {
	    bdfError("bad 'BBX'\n");
	    goto BAILOUT;
	}
	if ((bh < 0) || (bw < 0)) {
	    bdfError("character '%s' has a negative sized bitmap, %dx%d\n",
		     charName, bw, bh);
	    goto BAILOUT;
	}
	line = bdfGetLine(file, lineBuf, BDFLINELEN);
	if ((line) && (bdfIsPrefix(line, "ATTRIBUTES"))) {
	    for (p = line + strlen("ATTRIBUTES ");
		    (*p == ' ') || (*p == '\t');
		    p++)
		 /* empty for loop */ ;
	    ci->metrics.attributes = (bdfHexByte(p) << 8) + bdfHexByte(p + 2);
	    line = bdfGetLine(file, lineBuf, BDFLINELEN);
	} else
	    ci->metrics.attributes = 0;

	if (!line || !bdfIsPrefix(line, "BITMAP")) {
	    bdfError("missing 'BITMAP'\n");
	    goto BAILOUT;
	}
	/* collect data for generated properties */
	if ((strlen(charName) == 1)) {
	    if ((charName[0] >= '0') && (charName[0] <= '9')) {
		pState->digitWidths += wx;
		pState->digitCount++;
	    } else if (charName[0] == 'x') {
		pState->exHeight = (bh + bb) <= 0 ? bh : bh + bb;
	    }
	}
	if (!ignore) {
	    ci->metrics.leftSideBearing = bl;
	    ci->metrics.rightSideBearing = bl + bw;
	    ci->metrics.ascent = bh + bb;
	    ci->metrics.descent = -bb;
	    ci->metrics.characterWidth = wx;
	    ci->bits = NULL;
	    bdfReadBitmap(ci, file, bit, byte, glyph, scan, bitmapsSizes);
	    ci++;
	    ndx++;
	} else
	    bdfSkipBitmap(file, bh);

	line = bdfGetLine(file, lineBuf, BDFLINELEN);	/* get STARTCHAR or
							 * ENDFONT */
    }

    if (ndx + nignored != nchars) {
	bdfError("%d too few characters\n", nchars - (ndx + nignored));
	goto BAILOUT;
    }
    nchars = ndx;
    bitmapFont->num_chars = nchars;
    if ((line) && (bdfIsPrefix(line, "STARTCHAR"))) {
	bdfError("more characters than specified\n");
	goto BAILOUT;
    }
    if ((!line) || (!bdfIsPrefix(line, "ENDFONT"))) {
	bdfError("missing 'ENDFONT'\n");
	goto BAILOUT;
    }
    if (numEncodedGlyphs == 0)
	bdfWarning("No characters with valid encodings\n");

    nencoding = (pFont->info.lastRow - pFont->info.firstRow + 1) *
	(pFont->info.lastCol - pFont->info.firstCol + 1);
    bitmapFont->encoding = calloc(NUM_SEGMENTS(nencoding),sizeof(CharInfoPtr*));
    if (!bitmapFont->encoding) {
	bdfError("Couldn't allocate ppCI (%d,%d)\n",
                 NUM_SEGMENTS(nencoding),
                 (int) sizeof(CharInfoPtr*));
	goto BAILOUT;
    }
    pFont->info.allExist = TRUE;
    i = 0;
    for (char_row = pFont->info.firstRow;
	    char_row <= pFont->info.lastRow;
	    char_row++) {
	if (bdfEncoding[char_row] == (CharInfoPtr *) NULL) {
	    pFont->info.allExist = FALSE;
            i += pFont->info.lastCol - pFont->info.firstCol + 1;
	} else {
	    for (char_col = pFont->info.firstCol;
		    char_col <= pFont->info.lastCol;
		    char_col++) {
		if (!bdfEncoding[char_row][char_col])
		    pFont->info.allExist = FALSE;
                else {
                    if (!bitmapFont->encoding[SEGMENT_MAJOR(i)]) {
                        bitmapFont->encoding[SEGMENT_MAJOR(i)]=
                            calloc(BITMAP_FONT_SEGMENT_SIZE,
                                   sizeof(CharInfoPtr));
                        if (!bitmapFont->encoding[SEGMENT_MAJOR(i)])
                            goto BAILOUT;
                    }
                    ACCESSENCODINGL(bitmapFont->encoding,i) =
                        bdfEncoding[char_row][char_col];
                }
                i++;
            }
	}
    }
    for (i = 0; i < 256; i++)
	if (bdfEncoding[i])
	    free(bdfEncoding[i]);
    return (TRUE);
BAILOUT:
    for (i = 0; i < 256; i++)
	if (bdfEncoding[i])
	    free(bdfEncoding[i]);
    /* bdfFreeFontBits will clean up the rest */
    return (FALSE);
}
