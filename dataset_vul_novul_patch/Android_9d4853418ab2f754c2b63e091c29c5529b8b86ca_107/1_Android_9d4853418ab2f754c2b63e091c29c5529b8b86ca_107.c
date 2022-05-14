 normalize_color_encoding(color_encoding *encoding)
 {
   PNG_CONST double whiteY = encoding->red.Y + encoding->green.Y +
       encoding->blue.Y;
 
    if (whiteY != 1)
 {
      encoding->red.X /= whiteY;
      encoding->red.Y /= whiteY;
      encoding->red.Z /= whiteY;
      encoding->green.X /= whiteY;
      encoding->green.Y /= whiteY;
      encoding->green.Z /= whiteY;
      encoding->blue.X /= whiteY;
      encoding->blue.Y /= whiteY;
      encoding->blue.Z /= whiteY;
 }

 }
