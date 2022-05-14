BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtx* ctx)
{
	int bitmap_caret = 0;
	oTga *tga = NULL;
	/*	int pixel_block_size = 0;
		int image_block_size = 0; */
	volatile gdImagePtr image = NULL;
	int x = 0;
	int y = 0;

	tga = (oTga *) gdMalloc(sizeof(oTga));
	if (!tga) {
		return NULL;
	}

	tga->bitmap = NULL;
	tga->ident = NULL;

	if (read_header_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	/*TODO: Will this be used?
		pixel_block_size = tga->bits / 8;
		image_block_size = (tga->width * tga->height) * pixel_block_size;
	*/

	if (read_image_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	image = gdImageCreateTrueColor((int)tga->width, (int)tga->height );

	if (image == 0) {
		free_tga( tga );
		return NULL;
	}

	/*!	\brief Populate GD image object
	 *  Copy the pixel data from our tga bitmap buffer into the GD image
	 *  Disable blending and save the alpha channel per default
	 */
	if (tga->alphabits) {
		gdImageAlphaBlending(image, 0);
		gdImageSaveAlpha(image, 1);
	}

	/* TODO: use alphabits as soon as we support 24bit and other alpha bps (ie != 8bits) */
	for (y = 0; y < tga->height; y++) {
		register int *tpix = image->tpixels[y];
		for ( x = 0; x < tga->width; x++, tpix++) {
 			if (tga->bits == TGA_BPP_24) {
 				*tpix = gdTrueColor(tga->bitmap[bitmap_caret + 2], tga->bitmap[bitmap_caret + 1], tga->bitmap[bitmap_caret]);
 				bitmap_caret += 3;
			} else if (tga->bits == TGA_BPP_32 && tga->alphabits) {
 				register int a = tga->bitmap[bitmap_caret + 3];
 
 				*tpix = gdTrueColorAlpha(tga->bitmap[bitmap_caret + 2], tga->bitmap[bitmap_caret + 1], tga->bitmap[bitmap_caret], gdAlphaMax - (a >> 1));
				bitmap_caret += 4;
			}
		}
	}

	if (tga->flipv && tga->fliph) {
		gdImageFlipBoth(image);
	} else if (tga->flipv) {
		gdImageFlipVertical(image);
	} else if (tga->fliph) {
		gdImageFlipHorizontal(image);
	}

	free_tga(tga);

	return image;
}
