 static sk_sp<SkImage> newSkImageFromRaster(const SkImageInfo& info,
                                            PassRefPtr<Uint8Array> imagePixels,
                                           unsigned imageRowBytes) {
   SkPixmap pixmap(info, imagePixels->data(), imageRowBytes);
   return SkImage::MakeFromRaster(pixmap,
                                  [](const void*, void* pixels) {
                                   static_cast<Uint8Array*>(pixels)->deref();
                                 },
                                 imagePixels.leakRef());
 }
