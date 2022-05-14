gdImageScaleTwoPass(const gdImagePtr src, const unsigned int new_width,
                    const unsigned int new_height)
{
    const unsigned int src_width = src->sx;
    const unsigned int src_height = src->sy;
	gdImagePtr tmp_im = NULL;
	gdImagePtr dst = NULL;

    /* First, handle the trivial case. */
    if (src_width == new_width && src_height == new_height) {
        return gdImageClone(src);
    }/* if */

	/* Convert to truecolor if it isn't; this code requires it. */
	if (!src->trueColor) {
		gdImagePaletteToTrueColor(src);
	}/* if */

    /* Scale horizontally unless sizes are the same. */
    if (src_width == new_width) {
        tmp_im = src;
    } else {
        tmp_im = gdImageCreateTrueColor(new_width, src_height);
        if (tmp_im == NULL) {
            return NULL;
        }
        gdImageSetInterpolationMethod(tmp_im, src->interpolation_id);

        _gdScalePass(src, src_width, tmp_im, new_width, src_height, HORIZONTAL);
    }/* if .. else*/

    /* If vertical sizes match, we're done. */
    if (src_height == new_height) {
        assert(tmp_im != src);
        return tmp_im;
    }/* if */

    /* Otherwise, we need to scale vertically. */
	dst = gdImageCreateTrueColor(new_width, new_height);
	if (dst != NULL) {
        gdImageSetInterpolationMethod(dst, src->interpolation_id);
        _gdScalePass(tmp_im, src_height, dst, new_height, new_width, VERTICAL);
     }/* if */
 
     if (src != tmp_im) {
        gdImageDestroy(tmp_im);
     }/* if */
 
 	return dst;
}/* gdImageScaleTwoPass*/
