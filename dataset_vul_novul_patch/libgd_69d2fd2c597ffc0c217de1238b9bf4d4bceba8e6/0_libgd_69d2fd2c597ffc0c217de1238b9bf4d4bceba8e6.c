_gd2GetHeader (gdIOCtxPtr in, int *sx, int *sy,
               int *cs, int *vers, int *fmt, int *ncx, int *ncy,
               t_chunk_info ** chunkIdx)
{
	int i;
	int ch;
	char id[5];
	t_chunk_info *cidx;
	int sidx;
	int nc;

	GD2_DBG (printf ("Reading gd2 header info\n"));

	for (i = 0; i < 4; i++) {
		ch = gdGetC (in);
		if (ch == EOF) {
			goto fail1;
		};
		id[i] = ch;
	};
	id[4] = 0;

	GD2_DBG (printf ("Got file code: %s\n", id));

	/* Equiv. of 'magick'.  */
	if (strcmp (id, GD2_ID) != 0) {
		GD2_DBG (printf ("Not a valid gd2 file\n"));
		goto fail1;
	};

	/* Version */
	if (gdGetWord (vers, in) != 1) {
		goto fail1;
	};
	GD2_DBG (printf ("Version: %d\n", *vers));

	if ((*vers != 1) && (*vers != 2)) {
		GD2_DBG (printf ("Bad version: %d\n", *vers));
		goto fail1;
	};

	/* Image Size */
	if (!gdGetWord (sx, in)) {
		GD2_DBG (printf ("Could not get x-size\n"));
		goto fail1;
	}
	if (!gdGetWord (sy, in)) {
		GD2_DBG (printf ("Could not get y-size\n"));
		goto fail1;
	}
	GD2_DBG (printf ("Image is %dx%d\n", *sx, *sy));

	/* Chunk Size (pixels, not bytes!) */
	if (gdGetWord (cs, in) != 1) {
		goto fail1;
	};
	GD2_DBG (printf ("ChunkSize: %d\n", *cs));

	if ((*cs < GD2_CHUNKSIZE_MIN) || (*cs > GD2_CHUNKSIZE_MAX)) {
		GD2_DBG (printf ("Bad chunk size: %d\n", *cs));
		goto fail1;
	};

	/* Data Format */
	if (gdGetWord (fmt, in) != 1) {
		goto fail1;
	};
	GD2_DBG (printf ("Format: %d\n", *fmt));

	if ((*fmt != GD2_FMT_RAW) && (*fmt != GD2_FMT_COMPRESSED) &&
	        (*fmt != GD2_FMT_TRUECOLOR_RAW) &&
	        (*fmt != GD2_FMT_TRUECOLOR_COMPRESSED)) {
		GD2_DBG (printf ("Bad data format: %d\n", *fmt));
		goto fail1;
	};


	/* # of chunks wide */
	if (gdGetWord (ncx, in) != 1) {
		goto fail1;
	};
	GD2_DBG (printf ("%d Chunks Wide\n", *ncx));

	/* # of chunks high */
	if (gdGetWord (ncy, in) != 1) {
		goto fail1;
	};
 	GD2_DBG (printf ("%d Chunks vertically\n", *ncy));
 
 	if (gd2_compressed (*fmt)) {
		if (*ncx <= 0 || *ncy <= 0 || *ncx > INT_MAX / *ncy) {
			GD2_DBG(printf ("Illegal chunk counts: %d * %d\n", *ncx, *ncy));
			goto fail1;
		}
 		nc = (*ncx) * (*ncy);
 
 		GD2_DBG (printf ("Reading %d chunk index entries\n", nc));
		if (overflow2(sizeof(t_chunk_info), nc)) {
			goto fail1;
		}
		sidx = sizeof (t_chunk_info) * nc;
		if (sidx <= 0) {
			goto fail1;
		}

		cidx = gdCalloc (sidx, 1);
		if (cidx == NULL) {
			goto fail1;
		}
		for (i = 0; i < nc; i++) {
			if (gdGetInt (&cidx[i].offset, in) != 1) {
				goto fail2;
			};
			if (gdGetInt (&cidx[i].size, in) != 1) {
				goto fail2;
			};
			if (cidx[i].offset < 0 || cidx[i].size < 0)
				goto fail2;
		};
		*chunkIdx = cidx;
	};

	GD2_DBG (printf ("gd2 header complete\n"));

	return 1;
fail2:
	gdFree(cidx);
fail1:
	return 0;
}
