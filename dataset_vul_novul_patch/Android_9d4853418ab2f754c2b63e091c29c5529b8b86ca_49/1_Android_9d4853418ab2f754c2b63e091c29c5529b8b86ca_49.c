 static void gamma_transform_test(png_modifier *pm,
   PNG_CONST png_byte colour_type, PNG_CONST png_byte bit_depth,
   PNG_CONST int palette_number,
   PNG_CONST int interlace_type, PNG_CONST double file_gamma,
   PNG_CONST double screen_gamma, PNG_CONST png_byte sbit,
   PNG_CONST int use_input_precision, PNG_CONST int scale16)
 {
    size_t pos = 0;
    char name[64];

 if (sbit != bit_depth && sbit != 0)
 {
      pos = safecat(name, sizeof name, pos, "sbit(");
      pos = safecatn(name, sizeof name, pos, sbit);
      pos = safecat(name, sizeof name, pos, ") ");
 }

 else
      pos = safecat(name, sizeof name, pos, "gamma ");

 if (scale16)
      pos = safecat(name, sizeof name, pos, "16to8 ");

   pos = safecatd(name, sizeof name, pos, file_gamma, 3);
   pos = safecat(name, sizeof name, pos, "->");
   pos = safecatd(name, sizeof name, pos, screen_gamma, 3);

   gamma_test(pm, colour_type, bit_depth, palette_number, interlace_type,
      file_gamma, screen_gamma, sbit, 0, name, use_input_precision,
      scale16, pm->test_gamma_expand16, 0 , 0, 0);
}
