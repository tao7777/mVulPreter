process_bitmap_updates(STREAM s)
 {
	uint16 num_updates;
 	uint16 left, top, right, bottom, width, height;
	uint16 cx, cy, bpp, Bpp, compress, bufsize, size;
 	uint8 *data, *bmpdata;
	int i;
 	logger(Protocol, Debug, "%s()", __func__);
 
	in_uint16_le(s, num_updates);
 
	for (i = 0; i < num_updates; i++)
	{
		in_uint16_le(s, left);
		in_uint16_le(s, top);
		in_uint16_le(s, right);
		in_uint16_le(s, bottom);
		in_uint16_le(s, width);
		in_uint16_le(s, height);
		in_uint16_le(s, bpp);
		Bpp = (bpp + 7) / 8;
		in_uint16_le(s, compress);
		in_uint16_le(s, bufsize);
		cx = right - left + 1;
		cy = bottom - top + 1;
		logger(Graphics, Debug,
		       "process_bitmap_updates(), [%d,%d,%d,%d], [%d,%d], bpp=%d, compression=%d",
		       left, top, right, bottom, width, height, Bpp, compress);
		if (!compress)
		{
			int y;
			bmpdata = (uint8 *) xmalloc(width * height * Bpp);
			for (y = 0; y < height; y++)
			{
				in_uint8a(s, &bmpdata[(height - y - 1) * (width * Bpp)],
					  width * Bpp);
			}
			ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
			xfree(bmpdata);
			continue;
		}
 
 
		if (compress & 0x400)
		{
			size = bufsize;
		}
		else
		{
			in_uint8s(s, 2);	/* pad */
			in_uint16_le(s, size);
			in_uint8s(s, 4);	/* line_size, final_size */
		}
		in_uint8p(s, data, size);
 		bmpdata = (uint8 *) xmalloc(width * height * Bpp);
		if (bitmap_decompress(bmpdata, width, height, data, size, Bpp))
 		{
			ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
 		}
		else
		{
			logger(Graphics, Warning,
			       "process_bitmap_updates(), failed to decompress bitmap");
		}
 		xfree(bmpdata);
 	}
 }
