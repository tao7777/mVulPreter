uch *readpng_get_image(double display_exponent, int *pChannels, ulg *pRowbytes)
{
    ulg  rowbytes;


 /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */

 /* GRR WARNING:  grayscale needs to be expanded and channels reset! */

 *pRowbytes = rowbytes = channels*width;
 *pChannels = channels;

 if ((image_data = (uch *)malloc(rowbytes*height)) == NULL) {
 return NULL;
 }

 Trace((stderr, "readpng_get_image:  rowbytes = %ld, height = %ld\n", rowbytes, height));


 
     /* now we can go ahead and just read the whole image */
 
    if (fread(image_data, 1L, rowbytes*height, saved_infile) <
       rowbytes*height) {
        free (image_data);
        image_data = NULL;
        return NULL;
    }
 
     return image_data;
 }
