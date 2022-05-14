 bool ParamTraits<SkBitmap>::Read(const base::Pickle* m,
                                  base::PickleIterator* iter,
                                  SkBitmap* r) {
  const char* fixed_data;
  int fixed_data_size = 0;
  if (!iter->ReadData(&fixed_data, &fixed_data_size) ||
     (fixed_data_size <= 0)) {
     return false;
  }
  if (fixed_data_size != sizeof(SkBitmap_Data))
    return false;  // Message is malformed.
 
  const char* variable_data;
  int variable_data_size = 0;
  if (!iter->ReadData(&variable_data, &variable_data_size) ||
     (variable_data_size < 0)) {
     return false;
  }
  const SkBitmap_Data* bmp_data =
      reinterpret_cast<const SkBitmap_Data*>(fixed_data);
  return bmp_data->InitSkBitmapFromData(r, variable_data, variable_data_size);
 }
