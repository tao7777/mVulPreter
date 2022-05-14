PixarLogDecode(TIFF* tif, uint8* op, tmsize_t occ, uint16 s)
{
	static const char module[] = "PixarLogDecode";
	TIFFDirectory *td = &tif->tif_dir;
	PixarLogState* sp = DecoderState(tif);
	tmsize_t i;
	tmsize_t nsamples;
	int llen;
	uint16 *up;

	switch (sp->user_datafmt) {
	case PIXARLOGDATAFMT_FLOAT:
		nsamples = occ / sizeof(float);	/* XXX float == 32 bits */
		break;
	case PIXARLOGDATAFMT_16BIT:
	case PIXARLOGDATAFMT_12BITPICIO:
	case PIXARLOGDATAFMT_11BITLOG:
		nsamples = occ / sizeof(uint16); /* XXX uint16 == 16 bits */
		break;
	case PIXARLOGDATAFMT_8BIT:
	case PIXARLOGDATAFMT_8BITABGR:
		nsamples = occ;
		break;
	default:
		TIFFErrorExt(tif->tif_clientdata, module,
			"%d bit input not supported in PixarLog",
			td->td_bitspersample);
		return 0;
	}

	llen = sp->stride * td->td_imagewidth;

	(void) s;
	assert(sp != NULL);
	sp->stream.next_out = (unsigned char *) sp->tbuf;
	assert(sizeof(sp->stream.avail_out)==4);  /* if this assert gets raised,
	    we need to simplify this code to reflect a ZLib that is likely updated
	    to deal with 8byte memory sizes, though this code will respond
	    appropriately even before we simplify it */
	sp->stream.avail_out = (uInt) (nsamples * sizeof(uint16));
	if (sp->stream.avail_out != nsamples * sizeof(uint16))
	{
 		TIFFErrorExt(tif->tif_clientdata, module, "ZLib cannot deal with buffers this size");
 		return (0);
 	}
	/* Check that we will not fill more than what was allocated */
	if (sp->stream.avail_out > sp->tbuf_size)
	{
		TIFFErrorExt(tif->tif_clientdata, module, "sp->stream.avail_out > sp->tbuf_size");
		return (0);
	}
 	do {
 		int state = inflate(&sp->stream, Z_PARTIAL_FLUSH);
 		if (state == Z_STREAM_END) {
			break;			/* XXX */
		}
		if (state == Z_DATA_ERROR) {
			TIFFErrorExt(tif->tif_clientdata, module,
			    "Decoding error at scanline %lu, %s",
			    (unsigned long) tif->tif_row, sp->stream.msg ? sp->stream.msg : "(null)");
			if (inflateSync(&sp->stream) != Z_OK)
				return (0);
			continue;
		}
		if (state != Z_OK) {
			TIFFErrorExt(tif->tif_clientdata, module, "ZLib error: %s",
			    sp->stream.msg ? sp->stream.msg : "(null)");
			return (0);
		}
	} while (sp->stream.avail_out > 0);

	/* hopefully, we got all the bytes we needed */
	if (sp->stream.avail_out != 0) {
		TIFFErrorExt(tif->tif_clientdata, module,
		    "Not enough data at scanline %lu (short " TIFF_UINT64_FORMAT " bytes)",
		    (unsigned long) tif->tif_row, (TIFF_UINT64_T) sp->stream.avail_out);
		return (0);
	}

	up = sp->tbuf;
	/* Swap bytes in the data if from a different endian machine. */
	if (tif->tif_flags & TIFF_SWAB)
		TIFFSwabArrayOfShort(up, nsamples);

	/*
	 * if llen is not an exact multiple of nsamples, the decode operation
	 * may overflow the output buffer, so truncate it enough to prevent
	 * that but still salvage as much data as possible.
	 */
	if (nsamples % llen) { 
		TIFFWarningExt(tif->tif_clientdata, module,
			"stride %lu is not a multiple of sample count, "
			"%lu, data truncated.", (unsigned long) llen, (unsigned long) nsamples);
		nsamples -= nsamples % llen;
	}

	for (i = 0; i < nsamples; i += llen, up += llen) {
		switch (sp->user_datafmt)  {
		case PIXARLOGDATAFMT_FLOAT:
			horizontalAccumulateF(up, llen, sp->stride,
					(float *)op, sp->ToLinearF);
			op += llen * sizeof(float);
			break;
		case PIXARLOGDATAFMT_16BIT:
			horizontalAccumulate16(up, llen, sp->stride,
					(uint16 *)op, sp->ToLinear16);
			op += llen * sizeof(uint16);
			break;
		case PIXARLOGDATAFMT_12BITPICIO:
			horizontalAccumulate12(up, llen, sp->stride,
					(int16 *)op, sp->ToLinearF);
			op += llen * sizeof(int16);
			break;
		case PIXARLOGDATAFMT_11BITLOG:
			horizontalAccumulate11(up, llen, sp->stride,
					(uint16 *)op);
			op += llen * sizeof(uint16);
			break;
		case PIXARLOGDATAFMT_8BIT:
			horizontalAccumulate8(up, llen, sp->stride,
					(unsigned char *)op, sp->ToLinear8);
			op += llen * sizeof(unsigned char);
			break;
		case PIXARLOGDATAFMT_8BITABGR:
			horizontalAccumulate8abgr(up, llen, sp->stride,
					(unsigned char *)op, sp->ToLinear8);
			op += llen * sizeof(unsigned char);
			break;
		default:
			TIFFErrorExt(tif->tif_clientdata, module,
				  "Unsupported bits/sample: %d",
				  td->td_bitspersample);
			return (0);
		}
	}

	return (1);
}
