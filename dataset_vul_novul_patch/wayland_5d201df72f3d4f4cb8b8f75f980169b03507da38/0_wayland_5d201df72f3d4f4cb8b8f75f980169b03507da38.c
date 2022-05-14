XcursorImageCreate (int width, int height)
 {
     XcursorImage    *image;
 
    if (width < 0 || height < 0)
       return NULL;
    if (width > XCURSOR_IMAGE_MAX_SIZE || height > XCURSOR_IMAGE_MAX_SIZE)
       return NULL;

     image = malloc (sizeof (XcursorImage) +
 		    width * height * sizeof (XcursorPixel));
     if (!image)
    image->height = height;
    image->delay = 0;
    return image;
}
