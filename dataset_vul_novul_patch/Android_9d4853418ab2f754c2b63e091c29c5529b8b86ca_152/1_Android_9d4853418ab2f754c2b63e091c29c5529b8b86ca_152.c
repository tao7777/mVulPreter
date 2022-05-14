white_point(PNG_CONST color_encoding *encoding)
 {
    CIE_color white;
 
   white.X = encoding->red.X + encoding->green.X + encoding->blue.X;
   white.Y = encoding->red.Y + encoding->green.Y + encoding->blue.Y;
   white.Z = encoding->red.Z + encoding->green.Z + encoding->blue.Z;

 
    return white;
 }
