int read_image_tga( gdIOCtx *ctx, oTga *tga )
{
	int pixel_block_size = (tga->bits / 8);
	int image_block_size = (tga->width * tga->height) * pixel_block_size;
	uint8_t* decompression_buffer = NULL;
	unsigned char* conversion_buffer = NULL;
 	int buffer_caret = 0;
 	int bitmap_caret = 0;
 	int i = 0;
 	uint8_t encoded_pixels;
 
 	if(overflow2(tga->width, tga->height)) {
		return -1;
	}

	if(overflow2(tga->width * tga->height, pixel_block_size)) {
		return -1;
	}

	if(overflow2(image_block_size, sizeof(int))) {
		return -1;
	}

	/*! \todo Add more image type support.
	 */
	if (tga->imagetype != TGA_TYPE_RGB && tga->imagetype != TGA_TYPE_RGB_RLE)
		return -1;

	/*!	\brief Allocate memmory for image block
	 *  Allocate a chunk of memory for the image block to be passed into.
	 */
	tga->bitmap = (int *) gdMalloc(image_block_size * sizeof(int));
	if (tga->bitmap == NULL)
		return -1;

	switch (tga->imagetype) {
	case TGA_TYPE_RGB:
		/*! \brief Read in uncompressed RGB TGA
		 *  Chunk load the pixel data from an uncompressed RGB type TGA.
		 */
		conversion_buffer = (unsigned char *) gdMalloc(image_block_size * sizeof(unsigned char));
		if (conversion_buffer == NULL) {
			return -1;
		}

		if (gdGetBuf(conversion_buffer, image_block_size, ctx) != image_block_size) {
			gd_error("gd-tga: premature end of image data\n");
			gdFree(conversion_buffer);
			return -1;
		}

		while (buffer_caret < image_block_size) {
			tga->bitmap[buffer_caret] = (int) conversion_buffer[buffer_caret];
			buffer_caret++;
		}

		gdFree(conversion_buffer);
		break;

	case TGA_TYPE_RGB_RLE:
		/*! \brief Read in RLE compressed RGB TGA
		 *  Chunk load the pixel data from an RLE compressed RGB type TGA.
		 */
		decompression_buffer = (uint8_t*) gdMalloc(image_block_size * sizeof(uint8_t));
		if (decompression_buffer == NULL) {
			return -1;
		}
		conversion_buffer = (unsigned char *) gdMalloc(image_block_size * sizeof(unsigned char));
		if (conversion_buffer == NULL) {
			gd_error("gd-tga: premature end of image data\n");
			gdFree( decompression_buffer );
			return -1;
		}

		if (gdGetBuf(conversion_buffer, image_block_size, ctx) != image_block_size) {
			gdFree(conversion_buffer);
			gdFree(decompression_buffer);
			return -1;
		}

		buffer_caret = 0;

		while( buffer_caret < image_block_size) {
			decompression_buffer[buffer_caret] = (int)conversion_buffer[buffer_caret];
			buffer_caret++;
		}

		buffer_caret = 0;

 		while( bitmap_caret < image_block_size ) {
 			
 			if ((decompression_buffer[buffer_caret] & TGA_RLE_FLAG) == TGA_RLE_FLAG) {
				encoded_pixels = ( ( decompression_buffer[ buffer_caret ] & !TGA_RLE_FLAG ) + 1 );
 				buffer_caret++;
 
				if ((bitmap_caret + (encoded_pixels * pixel_block_size)) >= image_block_size) {
					gdFree( decompression_buffer );
					gdFree( conversion_buffer );
					return -1;
				}

				for (i = 0; i < encoded_pixels; i++) {
					memcpy(tga->bitmap + bitmap_caret, decompression_buffer + buffer_caret, pixel_block_size);
					bitmap_caret += pixel_block_size;
 				}
 				buffer_caret += pixel_block_size;

 			} else {
 				encoded_pixels = decompression_buffer[ buffer_caret ] + 1;
 				buffer_caret++;
 
				if ((bitmap_caret + (encoded_pixels * pixel_block_size)) >= image_block_size) {
					gdFree( decompression_buffer );
					gdFree( conversion_buffer );
					return -1;
 				}

				memcpy(tga->bitmap + bitmap_caret, decompression_buffer + buffer_caret, encoded_pixels * pixel_block_size);
				bitmap_caret += (encoded_pixels * pixel_block_size);
				buffer_caret += (encoded_pixels * pixel_block_size);
 			}
 		}
 		gdFree( decompression_buffer );
		gdFree( conversion_buffer );
		break;
	}

	return 1;
}
