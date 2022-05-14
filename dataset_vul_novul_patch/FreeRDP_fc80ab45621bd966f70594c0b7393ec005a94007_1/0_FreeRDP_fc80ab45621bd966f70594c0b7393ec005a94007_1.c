static BOOL region16_simplify_bands(REGION16* region)
{
	/** Simplify consecutive bands that touch and have the same items
	 *
	 *  ====================          ====================
	 *     | 1 |  | 2   |               |   |  |     |
	 *  ====================            |   |  |     |
	 *     | 1 |  | 2   |	   ====>    | 1 |  |  2  |
	 *  ====================            |   |  |     |
	 *     | 1 |  | 2   |               |   |  |     |
	 *  ====================          ====================
	 *
	 */
	RECTANGLE_16* band1, *band2, *endPtr, *endBand, *tmp;
	int nbRects, finalNbRects;
	int bandItems, toMove;
	finalNbRects = nbRects = region16_n_rects(region);

	if (nbRects < 2)
		return TRUE;

	band1 = region16_rects_noconst(region);
	endPtr = band1 + nbRects;

	do
	{
		band2 = next_band(band1, endPtr, &bandItems);

		if (band2 == endPtr)
			break;

		if ((band1->bottom == band2->top) && band_match(band1, band2, endPtr))
		{
			/* adjust the bottom of band1 items */
			tmp = band1;

			while (tmp < band2)
			{
				tmp->bottom = band2->bottom;
				tmp++;
			}

			/* override band2, we don't move band1 pointer as the band after band2
			 * may be merged too */
			endBand = band2 + bandItems;
			toMove = (endPtr - endBand) * sizeof(RECTANGLE_16);

			if (toMove)
				MoveMemory(band2, endBand, toMove);

			finalNbRects -= bandItems;
			endPtr -= bandItems;
		}
		else
		{
			band1 = band2;
		}
	}
	while (TRUE);
 
 	if (finalNbRects != nbRects)
 	{
		REGION16_DATA* data;
		size_t allocSize = sizeof(REGION16_DATA) + (finalNbRects * sizeof(RECTANGLE_16));
		data = realloc(region->data, allocSize);
		if (!data)
			free(region->data);
		region->data = data;
 
 		if (!region->data)
 		{
			region->data = &empty_region;
			return FALSE;
		}

		region->data->nbRects = finalNbRects;
		region->data->size = allocSize;
	}

	return TRUE;
}
