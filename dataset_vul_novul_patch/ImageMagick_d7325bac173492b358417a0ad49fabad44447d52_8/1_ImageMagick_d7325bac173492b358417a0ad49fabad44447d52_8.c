 static inline void VectorClamp3(DDSVector3 *value)
 {
  value->x = MinF(1.0f,MaxF(0.0f,value->x));
  value->y = MinF(1.0f,MaxF(0.0f,value->y));
  value->z = MinF(1.0f,MaxF(0.0f,value->z));
 }
