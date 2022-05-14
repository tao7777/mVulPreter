  void InitSkBitmapDataForTransfer(const SkBitmap& bitmap) {
    const SkImageInfo& info = bitmap.info();
    color_type = info.colorType();
    alpha_type = info.alphaType();
    width = info.width();
    height = info.height();
  }
