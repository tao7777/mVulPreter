 static sk_sp<SkImage> flipSkImageVertically(SkImage* input,
                                             AlphaDisposition alphaOp) {
  unsigned width = static_cast<unsigned>(input->width());
  unsigned height = static_cast<unsigned>(input->height());
   SkImageInfo info = SkImageInfo::MakeN32(input->width(), input->height(),
                                           (alphaOp == PremultiplyAlpha)
                                               ? kPremul_SkAlphaType
                                               : kUnpremul_SkAlphaType);
  unsigned imageRowBytes = width * info.bytesPerPixel();
   RefPtr<Uint8Array> imagePixels = copySkImageData(input, info);
   if (!imagePixels)
     return nullptr;
  for (unsigned i = 0; i < height / 2; i++) {
    unsigned topFirstElement = i * imageRowBytes;
    unsigned topLastElement = (i + 1) * imageRowBytes;
    unsigned bottomFirstElement = (height - 1 - i) * imageRowBytes;
     std::swap_ranges(imagePixels->data() + topFirstElement,
                      imagePixels->data() + topLastElement,
                      imagePixels->data() + bottomFirstElement);
  }
  return newSkImageFromRaster(info, std::move(imagePixels), imageRowBytes);
}
