image_transform_png_set_@_add(image_transform *this,
    PNG_CONST image_transform **that, char *name, size_t sizeof_name,
    size_t *pos, png_byte colour_type, png_byte bit_depth)
{
   this->next = *that;
   *that = this;
   *pos = safecat(name, sizeof_name, *pos, " +@");
   return 1;
}
