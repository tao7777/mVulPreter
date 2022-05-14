process_bitmap_updates(STREAM s)
/* Process TS_BITMAP_DATA */
static void
process_bitmap_data(STREAM s)
 {
 	uint16 left, top, right, bottom, width, height;
	uint16 cx, cy, bpp, Bpp, flags, bufsize, size;
 	uint8 *data, *bmpdata;
	
 	logger(Protocol, Debug, "%s()", __func__);
 
	struct stream packet = *s;
 
	in_uint16_le(s, left); /* destLeft */
	in_uint16_le(s, top); /* destTop */
	in_uint16_le(s, right); /* destRight */
	in_uint16_le(s, bottom); /* destBottom */
	in_uint16_le(s, width); /* width */
	in_uint16_le(s, height); /* height */
	in_uint16_le(s, bpp); /*bitsPerPixel */
	Bpp = (bpp + 7) / 8;
	in_uint16_le(s, flags); /* flags */
	in_uint16_le(s, bufsize); /* bitmapLength */
 
	cx = right - left + 1;
	cy = bottom - top + 1;
 
	/* FIXME: There are a assumtion that we do not consider in
		this code. The value of bpp is not passed to
		ui_paint_bitmap() which relies on g_server_bpp for drawing
		the bitmap data.

		Does this means that we can sanity check bpp with g_server_bpp ?
	*/

	if (Bpp == 0 || width == 0 || height == 0)
	{
        logger(Protocol, Warning, "%s(), [%d,%d,%d,%d], [%d,%d], bpp=%d, flags=%x", __func__,
				left, top, right, bottom, width, height, bpp, flags);
		rdp_protocol_error("TS_BITMAP_DATA, unsafe size of bitmap data received from server", &packet);
	}

	if ((RD_UINT32_MAX / Bpp) <= (width * height))
	{
		logger(Protocol, Warning, "%s(), [%d,%d,%d,%d], [%d,%d], bpp=%d, flags=%x", __func__,
				left, top, right, bottom, width, height, bpp, flags);
		rdp_protocol_error("TS_BITMAP_DATA, unsafe size of bitmap data received from server", &packet);
	}
 
	if (flags == 0)
	{
		/* read uncompressed bitmap data */
		int y;
 		bmpdata = (uint8 *) xmalloc(width * height * Bpp);
		for (y = 0; y < height; y++)
 		{
			in_uint8a(s, &bmpdata[(height - y - 1) * (width * Bpp)], width * Bpp);
 		}
		
		ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
 		xfree(bmpdata);
		return;
	}

	if (flags & NO_BITMAP_COMPRESSION_HDR)
	{
		size = bufsize;
	}
	else
	{
		/* Read TS_CD_HEADER */
		in_uint8s(s, 2);        /* skip cbCompFirstRowSize (must be 0x0000) */
		in_uint16_le(s, size);  /* cbCompMainBodySize */
		in_uint8s(s, 2);        /* skip cbScanWidth */
		in_uint8s(s, 2);        /* skip cbUncompressedSize */
	}

	/* read compressed bitmap data */
	if (!s_check_rem(s, size))
	{
		rdp_protocol_error("process_bitmap_data(), consume of bitmap data from stream would overrun", &packet);
	}
	in_uint8p(s, data, size);
	bmpdata = (uint8 *) xmalloc(width * height * Bpp);
	if (bitmap_decompress(bmpdata, width, height, data, size, Bpp))
	{
		ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
	}
	else
	{
		logger(Protocol, Warning, "%s(), failed to decompress bitmap", __func__);
	}

	xfree(bmpdata);
}

/* Process TS_UPDATE_BITMAP_DATA */
void
process_bitmap_updates(STREAM s)
{
	int i;
	uint16 num_updates;
	
	in_uint16_le(s, num_updates);   /* rectangles */

	for (i = 0; i < num_updates; i++)
	{
		process_bitmap_data(s);
 	}
 }
