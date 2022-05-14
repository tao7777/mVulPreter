 static sk_sp<SkImage> flipSkImageVertically(SkImage* input,
                                             AlphaDisposition alphaOp) {
  size_t width = static_cast<size_t>(input->width());
  size_t height = static_cast<size_t>(input->height());
   SkImageInfo info = SkImageInfo::MakeN32(input->width(), input->height(),
                                           (alphaOp == PremultiplyAlpha)
                                               ? kPremul_SkAlphaType
                                               : kUnpremul_SkAlphaType);
  size_t imageRowBytes = width * info.bytesPerPixel();
   RefPtr<Uint8Array> imagePixels = copySkImageData(input, info);
   if (!imagePixels)
     return nullptr;
  for (size_t i = 0; i < height / 2; i++) {
    size_t topFirstElement = i * imageRowBytes;
    size_t topLastElement = (i + 1) * imageRowBytes;
    size_t bottomFirstElement = (height - 1 - i) * imageRowBytes;
     std::swap_ranges(imagePixels->data() + topFirstElement,
                      imagePixels->data() + topLastElement,
                      imagePixels->data() + bottomFirstElement);
  }
  return newSkImageFromRaster(info, std::move(imagePixels), imageRowBytes);
}
