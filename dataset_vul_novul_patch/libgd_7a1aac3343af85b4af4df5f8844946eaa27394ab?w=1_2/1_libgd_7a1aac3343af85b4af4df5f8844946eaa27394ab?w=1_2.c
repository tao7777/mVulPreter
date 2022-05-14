int gdTransformAffineGetImage(gdImagePtr *dst,
		  const gdImagePtr src,
		  gdRectPtr src_area,
		  const double affine[6])
{
	int res;
	double m[6];
	gdRect bbox;
	gdRect area_full;

	if (src_area == NULL) {
		area_full.x = 0;
		area_full.y = 0;
		area_full.width  = gdImageSX(src);
		area_full.height = gdImageSY(src);
		src_area = &area_full;
	}

	gdTransformAffineBoundingBox(src_area, affine, &bbox);

	*dst = gdImageCreateTrueColor(bbox.width, bbox.height);
	if (*dst == NULL) {
		return GD_FALSE;
	}
	(*dst)->saveAlphaFlag = 1;

 	if (!src->trueColor) {
 		gdImagePaletteToTrueColor(src);
 	}
 	/* Translate to dst origin (0,0) */
 	gdAffineTranslate(m, -bbox.x, -bbox.y);
 	gdAffineConcat(m, affine, m);

	gdImageAlphaBlending(*dst, 0);

	res = gdTransformAffineCopy(*dst,
		  0,0,
		  src,
		  src_area,
		  m);

	if (res != GD_TRUE) {
		gdImageDestroy(*dst);
		dst = NULL;
		return GD_FALSE;
	} else {
		return GD_TRUE;
	}
}
