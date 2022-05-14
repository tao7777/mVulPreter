XcursorImageCreate (int width, int height)
 {
     XcursorImage    *image;
 
     image = malloc (sizeof (XcursorImage) +
 		    width * height * sizeof (XcursorPixel));
     if (!image)
    image->height = height;
    image->delay = 0;
    return image;
}
