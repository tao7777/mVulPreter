 BOOL region16_union_rect(REGION16* dst, const REGION16* src, const RECTANGLE_16* rect)
 {
	REGION16_DATA* data;
 	const RECTANGLE_16* srcExtents;
 	RECTANGLE_16* dstExtents;
 	const RECTANGLE_16* currentBand, *endSrcRect, *nextBand;
	REGION16_DATA* newItems = NULL;
	RECTANGLE_16* dstRect = NULL;
	UINT32 usedRects, srcNbRects;
	UINT16 topInterBand;
	assert(src);
	assert(src->data);
	assert(dst);
	srcExtents = region16_extents(src);
	dstExtents = region16_extents_noconst(dst);

	if (!region16_n_rects(src))
	{
		/* source is empty, so the union is rect */
		dst->extents = *rect;
		dst->data = allocateRegion(1);

		if (!dst->data)
			return FALSE;

		dstRect = region16_rects_noconst(dst);
		dstRect->top = rect->top;
		dstRect->left = rect->left;
		dstRect->right = rect->right;
		dstRect->bottom = rect->bottom;
		return TRUE;
	}

	newItems = allocateRegion((1 + region16_n_rects(src)) * 4);

	if (!newItems)
		return FALSE;

	dstRect = (RECTANGLE_16*)(&newItems[1]);
	usedRects = 0;

	/* adds the piece of rect that is on the top of src */
	if (rect->top < srcExtents->top)
	{
		dstRect->top = rect->top;
		dstRect->left = rect->left;
		dstRect->right = rect->right;
		dstRect->bottom = MIN(srcExtents->top, rect->bottom);
		usedRects++;
		dstRect++;
	}

	/* treat possibly overlapping region */
	currentBand = region16_rects(src, &srcNbRects);
	endSrcRect = currentBand + srcNbRects;

	while (currentBand < endSrcRect)
	{
		if ((currentBand->bottom <= rect->top) || (rect->bottom <= currentBand->top) ||
		    rectangle_contained_in_band(currentBand, endSrcRect, rect))
		{
			/* no overlap between rect and the band, rect is totally below or totally above
			 * the current band, or rect is already covered by an item of the band.
			 * let's copy all the rectangles from this band
						+----+
						|    |   rect (case 1)
						+----+

			   =================
			band of srcRect
			 =================
					+----+
					|    |   rect (case 2)
					+----+
			*/
			region16_copy_band_with_union(dstRect,
			                              currentBand, endSrcRect,
			                              currentBand->top, currentBand->bottom,
			                              NULL, &usedRects,
			                              &nextBand, &dstRect);
			topInterBand = rect->top;
		}
		else
		{
			/* rect overlaps the band:
					   |    |  |    |
			====^=================|    |==|    |=========================== band
			|   top split     |    |  |    |
			v                 | 1  |  | 2  |
			^                 |    |  |    |  +----+   +----+
			|   merge zone    |    |  |    |  |    |   | 4  |
			v                 +----+  |    |  |    |   +----+
			^                         |    |  | 3  |
			|   bottom split          |    |  |    |
			====v=========================|    |==|    |===================
					   |    |  |    |

			 possible cases:
			 1) no top split, merge zone then a bottom split. The band will be splitted
			  in two
			 2) not band split, only the merge zone, band merged with rect but not splitted
			 3) a top split, the merge zone and no bottom split. The band will be split
			 in two
			 4) a top split, the merge zone and also a bottom split. The band will be
			 splitted in 3, but the coalesce algorithm may merge the created bands
			 */
			UINT16 mergeTop = currentBand->top;
			UINT16 mergeBottom = currentBand->bottom;

			/* test if we need a top split, case 3 and 4 */
			if (rect->top > currentBand->top)
			{
				region16_copy_band_with_union(dstRect,
				                              currentBand, endSrcRect,
				                              currentBand->top, rect->top,
				                              NULL, &usedRects,
				                              &nextBand, &dstRect);
				mergeTop = rect->top;
			}

			/* do the merge zone (all cases) */
			if (rect->bottom < currentBand->bottom)
				mergeBottom = rect->bottom;

			region16_copy_band_with_union(dstRect,
			                              currentBand, endSrcRect,
			                              mergeTop, mergeBottom,
			                              rect, &usedRects,
			                              &nextBand, &dstRect);

			/* test if we need a bottom split, case 1 and 4 */
			if (rect->bottom < currentBand->bottom)
			{
				region16_copy_band_with_union(dstRect,
				                              currentBand, endSrcRect,
				                              mergeBottom, currentBand->bottom,
				                              NULL, &usedRects,
				                              &nextBand, &dstRect);
			}

			topInterBand = currentBand->bottom;
		}

		/* test if a piece of rect should be inserted as a new band between
		 * the current band and the next one. band n and n+1 shouldn't touch.
		 *
		 * ==============================================================
		 *                                                        band n
		 *            +------+                    +------+
		 * ===========| rect |====================|      |===============
		 *            |      |    +------+        |      |
		 *            +------+    | rect |        | rect |
		 *                        +------+        |      |
		 * =======================================|      |================
		 *                                        +------+         band n+1
		 * ===============================================================
		 *
		 */
		if ((nextBand < endSrcRect) && (nextBand->top != currentBand->bottom) &&
		    (rect->bottom > currentBand->bottom) && (rect->top < nextBand->top))
		{
			dstRect->right = rect->right;
			dstRect->left = rect->left;
			dstRect->top = topInterBand;
			dstRect->bottom = MIN(nextBand->top, rect->bottom);
			dstRect++;
			usedRects++;
		}

		currentBand = nextBand;
	}

	/* adds the piece of rect that is below src */
	if (srcExtents->bottom < rect->bottom)
	{
		dstRect->top = MAX(srcExtents->bottom, rect->top);
		dstRect->left = rect->left;
		dstRect->right = rect->right;
		dstRect->bottom = rect->bottom;
		usedRects++;
		dstRect++;
	}

	if ((src == dst) && (src->data->size > 0) && (src->data != &empty_region))
		free(src->data);

	dstExtents->top = MIN(rect->top, srcExtents->top);
	dstExtents->left = MIN(rect->left, srcExtents->left);
 	dstExtents->bottom = MAX(rect->bottom, srcExtents->bottom);
 	dstExtents->right = MAX(rect->right, srcExtents->right);
 	newItems->size = sizeof(REGION16_DATA) + (usedRects * sizeof(RECTANGLE_16));
	data = realloc(newItems, newItems->size);
	if (!data)
		free(dst->data);
	dst->data = data;
 
 	if (!dst->data)
 	{
		free(newItems);
		return FALSE;
	}

	dst->data->nbRects = usedRects;
	return region16_simplify_bands(dst);
}
