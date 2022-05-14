 bool ParamTraits<SkBitmap>::Read(const base::Pickle* m,
                                  base::PickleIterator* iter,
                                  SkBitmap* r) {
  SkImageInfo image_info;
  if (!ReadParam(m, iter, &image_info))
     return false;
 
  const char* bitmap_data;
  int bitmap_data_size = 0;
  if (!iter->ReadData(&bitmap_data, &bitmap_data_size))
     return false;
  // ReadData() only returns true if bitmap_data_size >= 0.

  if (!r->tryAllocPixels(image_info))
    return false;

  if (static_cast<size_t>(bitmap_data_size) != r->computeByteSize())
    return false;
  memcpy(r->getPixels(), bitmap_data, bitmap_data_size);
  return true;
 }
