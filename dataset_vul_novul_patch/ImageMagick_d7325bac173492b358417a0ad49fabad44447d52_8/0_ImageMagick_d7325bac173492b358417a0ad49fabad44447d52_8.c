 static inline void VectorClamp3(DDSVector3 *value)
 {
  value->x = MagickMin(1.0f,MagickMax(0.0f,value->x));
  value->y = MagickMin(1.0f,MagickMax(0.0f,value->y));
  value->z = MagickMin(1.0f,MagickMax(0.0f,value->z));
 }
