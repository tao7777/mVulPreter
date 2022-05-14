 BOOL region16_intersect_rect(REGION16* dst, const REGION16* src, const RECTANGLE_16* rect)
 {
	REGION16_DATA* data;
 	REGION16_DATA* newItems;
 	const RECTANGLE_16* srcPtr, *endPtr, *srcExtents;
 	RECTANGLE_16* dstPtr;
	UINT32 nbRects, usedRects;
	RECTANGLE_16 common, newExtents;
	assert(src);
	assert(src->data);
	srcPtr = region16_rects(src, &nbRects);

	if (!nbRects)
	{
		region16_clear(dst);
		return TRUE;
	}

	srcExtents = region16_extents(src);

	if (nbRects == 1)
	{
		BOOL intersects = rectangles_intersection(srcExtents, rect, &common);
		region16_clear(dst);

		if (intersects)
			return region16_union_rect(dst, dst, &common);

		return TRUE;
	}

	newItems = allocateRegion(nbRects);

	if (!newItems)
		return FALSE;

	dstPtr = (RECTANGLE_16*)(&newItems[1]);
	usedRects = 0;
	ZeroMemory(&newExtents, sizeof(newExtents));

	/* accumulate intersecting rectangles, the final region16_simplify_bands() will
	 * do all the bad job to recreate correct rectangles
	 */
	for (endPtr = srcPtr + nbRects; (srcPtr < endPtr) && (rect->bottom > srcPtr->top); srcPtr++)
	{
		if (rectangles_intersection(srcPtr, rect, &common))
		{
			*dstPtr = common;
			usedRects++;
			dstPtr++;

			if (rectangle_is_empty(&newExtents))
			{
				/* Check if the existing newExtents is empty. If it is empty, use
				 * new common directly. We do not need to check common rectangle
				 * because the rectangles_intersection() ensures that it is not empty.
				 */
				newExtents = common;
			}
			else
			{
				newExtents.top = MIN(common.top, newExtents.top);
				newExtents.left = MIN(common.left, newExtents.left);
				newExtents.bottom = MAX(common.bottom, newExtents.bottom);
				newExtents.right = MAX(common.right, newExtents.right);
			}
		}
	}

	newItems->nbRects = usedRects;
	newItems->size = sizeof(REGION16_DATA) + (usedRects * sizeof(RECTANGLE_16));

 	if ((dst->data->size > 0) && (dst->data != &empty_region))
 		free(dst->data);
 
	data = realloc(newItems, newItems->size);
	if (!data)
		free(dst->data);
	dst->data = data;
 
 	if (!dst->data)
 	{
		free(newItems);
		return FALSE;
	}

	dst->extents = newExtents;
	return region16_simplify_bands(dst);
}
