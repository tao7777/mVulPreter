static sk_sp<SkImage> scaleSkImage(sk_sp<SkImage> skImage,
                                   unsigned resizeWidth,
                                   unsigned resizeHeight,
                                   SkFilterQuality resizeQuality) {
  SkImageInfo resizedInfo = SkImageInfo::Make(
      resizeWidth, resizeHeight, kN32_SkColorType, kUnpremul_SkAlphaType);
  RefPtr<ArrayBuffer> dstBuffer = ArrayBuffer::createOrNull(
      resizeWidth * resizeHeight, resizedInfo.bytesPerPixel());
  if (!dstBuffer)
    return nullptr;
  RefPtr<Uint8Array> resizedPixels =
       Uint8Array::create(dstBuffer, 0, dstBuffer->byteLength());
   SkPixmap pixmap(
       resizedInfo, resizedPixels->data(),
      static_cast<unsigned>(resizeWidth) * resizedInfo.bytesPerPixel());
   skImage->scalePixels(pixmap, resizeQuality);
   return SkImage::MakeFromRaster(pixmap,
                                  [](const void*, void* pixels) {
                                   static_cast<Uint8Array*>(pixels)->deref();
                                 },
                                 resizedPixels.release().leakRef());
}
