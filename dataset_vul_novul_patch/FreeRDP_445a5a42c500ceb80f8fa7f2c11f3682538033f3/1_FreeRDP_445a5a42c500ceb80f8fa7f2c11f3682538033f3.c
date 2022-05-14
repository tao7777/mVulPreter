BITMAP_UPDATE* update_read_bitmap_update(rdpUpdate* update, wStream* s)
{
	UINT32 i;
	BITMAP_UPDATE* bitmapUpdate = calloc(1, sizeof(BITMAP_UPDATE));

	if (!bitmapUpdate)
		goto fail;

	if (Stream_GetRemainingLength(s) < 2)
		goto fail;

	Stream_Read_UINT16(s, bitmapUpdate->number); /* numberRectangles (2 bytes) */
	WLog_Print(update->log, WLOG_TRACE, "BitmapUpdate: %"PRIu32"", bitmapUpdate->number);
 
 	if (bitmapUpdate->number > bitmapUpdate->count)
 	{
		UINT16 count;
		BITMAP_DATA* newdata;
		count = bitmapUpdate->number * 2;
		newdata = (BITMAP_DATA*) realloc(bitmapUpdate->rectangles,
		                                 sizeof(BITMAP_DATA) * count);
 
 		if (!newdata)
 			goto fail;

		bitmapUpdate->rectangles = newdata;
		ZeroMemory(&bitmapUpdate->rectangles[bitmapUpdate->count],
		           sizeof(BITMAP_DATA) * (count - bitmapUpdate->count));
		bitmapUpdate->count = count;
	}

	/* rectangles */
	for (i = 0; i < bitmapUpdate->number; i++)
	{
		if (!update_read_bitmap_data(update, s, &bitmapUpdate->rectangles[i]))
			goto fail;
	}

	return bitmapUpdate;
fail:
	free_bitmap_update(update->context, bitmapUpdate);
	return NULL;
}
