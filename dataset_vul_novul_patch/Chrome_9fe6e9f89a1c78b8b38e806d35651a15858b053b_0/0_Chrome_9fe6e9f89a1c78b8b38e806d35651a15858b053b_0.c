  bool InitSkBitmapFromData(SkBitmap* bitmap,
bool ParamTraits<SkImageInfo>::Read(const base::Pickle* m,
                                    base::PickleIterator* iter,
                                    SkImageInfo* r) {
  SkColorType color_type;
  SkAlphaType alpha_type;
  uint32_t width;
  uint32_t height;
  if (!ReadParam(m, iter, &color_type) || !ReadParam(m, iter, &alpha_type) ||
      !ReadParam(m, iter, &width) || !ReadParam(m, iter, &height)) {
    return false;
   }
