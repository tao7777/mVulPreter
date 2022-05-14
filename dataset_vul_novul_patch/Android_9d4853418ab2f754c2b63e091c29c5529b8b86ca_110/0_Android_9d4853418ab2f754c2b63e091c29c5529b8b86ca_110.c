static int output_quantization_factor(PNG_CONST png_modifier *pm, int in_depth,
static int output_quantization_factor(const png_modifier *pm, int in_depth,
    int out_depth)
 {
    if (out_depth == 16 && in_depth != 16 &&
      pm->calculations_use_input_precision)
 return 257;
 else
 return 1;
}
