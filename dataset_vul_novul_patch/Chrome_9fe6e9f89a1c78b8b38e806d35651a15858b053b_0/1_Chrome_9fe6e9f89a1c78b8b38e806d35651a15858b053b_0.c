  bool InitSkBitmapFromData(SkBitmap* bitmap,
                            const char* pixels,
                            size_t pixels_size) const {
    if (!bitmap->tryAllocPixels(
            SkImageInfo::Make(width, height, color_type, alpha_type)))
      return false;
    if (pixels_size != bitmap->computeByteSize())
      return false;
    memcpy(bitmap->getPixels(), pixels, pixels_size);
    return true;
   }
