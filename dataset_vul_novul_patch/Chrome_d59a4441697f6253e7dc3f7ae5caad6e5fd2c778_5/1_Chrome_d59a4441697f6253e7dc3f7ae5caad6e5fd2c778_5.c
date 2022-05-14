static sk_sp<SkImage> premulSkImageToUnPremul(SkImage* input) {
  SkImageInfo info = SkImageInfo::Make(input->width(), input->height(),
                                       kN32_SkColorType, kUnpremul_SkAlphaType);
  RefPtr<Uint8Array> dstPixels = copySkImageData(input, info);
  if (!dstPixels)
     return nullptr;
   return newSkImageFromRaster(
       info, std::move(dstPixels),
      static_cast<size_t>(input->width()) * info.bytesPerPixel());
 }
