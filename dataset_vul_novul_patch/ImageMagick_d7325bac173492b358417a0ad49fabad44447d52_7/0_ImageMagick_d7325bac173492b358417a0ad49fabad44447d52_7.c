 static inline void VectorClamp(DDSVector4 *value)
 {
  value->x = MagickMin(1.0f,MagickMax(0.0f,value->x));
  value->y = MagickMin(1.0f,MagickMax(0.0f,value->y));
  value->z = MagickMin(1.0f,MagickMax(0.0f,value->z));
  value->w = MagickMin(1.0f,MagickMax(0.0f,value->w));
 }
