DECLAREcpFunc(cpContig2SeparateByRow)
{
	tsize_t scanlinesizein = TIFFScanlineSize(in);
	tsize_t scanlinesizeout = TIFFScanlineSize(out);
	tdata_t inbuf;
	tdata_t outbuf;
	register uint8 *inp, *outp;
 	register uint32 n;
 	uint32 row;
 	tsample_t s;
        uint16 bps = 0;

        (void) TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bps);
        if( bps != 8 )
        {
            TIFFError(TIFFFileName(in),
                      "Error, can only handle BitsPerSample=8 in %s",
                      "cpContig2SeparateByRow");
            return 0;
        }
 
 	inbuf = _TIFFmalloc(scanlinesizein);
 	outbuf = _TIFFmalloc(scanlinesizeout);
	if (!inbuf || !outbuf)
		goto bad;
	_TIFFmemset(inbuf, 0, scanlinesizein);
	_TIFFmemset(outbuf, 0, scanlinesizeout);
	/* unpack channels */
	for (s = 0; s < spp; s++) {
		for (row = 0; row < imagelength; row++) {
			if (TIFFReadScanline(in, inbuf, row, 0) < 0
			    && !ignore) {
				TIFFError(TIFFFileName(in),
				    "Error, can't read scanline %lu",
				    (unsigned long) row);
				goto bad;
			}
			inp = ((uint8*)inbuf) + s;
			outp = (uint8*)outbuf;
			for (n = imagewidth; n-- > 0;) {
				*outp++ = *inp;
				inp += spp;
			}
			if (TIFFWriteScanline(out, outbuf, row, s) < 0) {
				TIFFError(TIFFFileName(out),
				    "Error, can't write scanline %lu",
				    (unsigned long) row);
				goto bad;
			}
		}
	}
	if (inbuf) _TIFFfree(inbuf);
	if (outbuf) _TIFFfree(outbuf);
	return 1;
bad:
	if (inbuf) _TIFFfree(inbuf);
	if (outbuf) _TIFFfree(outbuf);
	return 0;
}
