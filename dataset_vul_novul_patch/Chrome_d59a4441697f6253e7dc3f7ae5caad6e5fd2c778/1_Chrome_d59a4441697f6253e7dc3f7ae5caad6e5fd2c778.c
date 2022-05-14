ImageBitmap::ImageBitmap(ImageData* data,
                         Optional<IntRect> cropRect,
                         const ImageBitmapOptions& options) {
  IntRect dataSrcRect = IntRect(IntPoint(), data->size());
  ParsedOptions parsedOptions =
      parseOptions(options, cropRect, data->bitmapSourceSize());
  if (dstBufferSizeHasOverflow(parsedOptions))
    return;
  IntRect srcRect = cropRect ? intersection(parsedOptions.cropRect, dataSrcRect)
                             : dataSrcRect;

  if (!parsedOptions.premultiplyAlpha) {
    unsigned char* srcAddr = data->data()->data();

     SkImageInfo info = SkImageInfo::Make(
         parsedOptions.cropRect.width(), parsedOptions.cropRect.height(),
         kN32_SkColorType, kUnpremul_SkAlphaType);
    size_t bytesPerPixel = static_cast<size_t>(info.bytesPerPixel());
    size_t srcPixelBytesPerRow = bytesPerPixel * data->size().width();
    size_t dstPixelBytesPerRow = bytesPerPixel * parsedOptions.cropRect.width();
     sk_sp<SkImage> skImage;
     if (parsedOptions.cropRect == IntRect(IntPoint(), data->size())) {
       swizzleImageData(srcAddr, data->size().height(), srcPixelBytesPerRow,
                       parsedOptions.flipY);
      skImage =
          SkImage::MakeRasterCopy(SkPixmap(info, srcAddr, dstPixelBytesPerRow));
      swizzleImageData(srcAddr, data->size().height(), srcPixelBytesPerRow,
                        parsedOptions.flipY);
     } else {
       RefPtr<ArrayBuffer> dstBuffer = ArrayBuffer::createOrNull(
          static_cast<size_t>(parsedOptions.cropRect.height()) *
               parsedOptions.cropRect.width(),
           bytesPerPixel);
       if (!dstBuffer)
        return;
      RefPtr<Uint8Array> copiedDataBuffer =
          Uint8Array::create(dstBuffer, 0, dstBuffer->byteLength());
      if (!srcRect.isEmpty()) {
        IntPoint srcPoint = IntPoint(
            (parsedOptions.cropRect.x() > 0) ? parsedOptions.cropRect.x() : 0,
            (parsedOptions.cropRect.y() > 0) ? parsedOptions.cropRect.y() : 0);
        IntPoint dstPoint = IntPoint(
            (parsedOptions.cropRect.x() >= 0) ? 0 : -parsedOptions.cropRect.x(),
            (parsedOptions.cropRect.y() >= 0) ? 0
                                              : -parsedOptions.cropRect.y());
        int copyHeight = data->size().height() - srcPoint.y();
        if (parsedOptions.cropRect.height() < copyHeight)
          copyHeight = parsedOptions.cropRect.height();
        int copyWidth = data->size().width() - srcPoint.x();
         if (parsedOptions.cropRect.width() < copyWidth)
           copyWidth = parsedOptions.cropRect.width();
         for (int i = 0; i < copyHeight; i++) {
          size_t srcStartCopyPosition =
               (i + srcPoint.y()) * srcPixelBytesPerRow +
               srcPoint.x() * bytesPerPixel;
          size_t srcEndCopyPosition =
               srcStartCopyPosition + copyWidth * bytesPerPixel;
          size_t dstStartCopyPosition;
           if (parsedOptions.flipY)
             dstStartCopyPosition =
                 (parsedOptions.cropRect.height() - 1 - dstPoint.y() - i) *
                    dstPixelBytesPerRow +
                dstPoint.x() * bytesPerPixel;
           else
             dstStartCopyPosition = (dstPoint.y() + i) * dstPixelBytesPerRow +
                                    dstPoint.x() * bytesPerPixel;
          for (size_t j = 0; j < srcEndCopyPosition - srcStartCopyPosition;
                j++) {
             if (kN32_SkColorType == kBGRA_8888_SkColorType) {
              if (j % 4 == 0)
                copiedDataBuffer->data()[dstStartCopyPosition + j] =
                    srcAddr[srcStartCopyPosition + j + 2];
              else if (j % 4 == 2)
                copiedDataBuffer->data()[dstStartCopyPosition + j] =
                    srcAddr[srcStartCopyPosition + j - 2];
              else
                copiedDataBuffer->data()[dstStartCopyPosition + j] =
                    srcAddr[srcStartCopyPosition + j];
            } else {
              copiedDataBuffer->data()[dstStartCopyPosition + j] =
                  srcAddr[srcStartCopyPosition + j];
            }
          }
        }
      }
      skImage = newSkImageFromRaster(info, std::move(copiedDataBuffer),
                                     dstPixelBytesPerRow);
    }
    if (!skImage)
      return;
    if (parsedOptions.shouldScaleInput)
      m_image = StaticBitmapImage::create(scaleSkImage(
          skImage, parsedOptions.resizeWidth, parsedOptions.resizeHeight,
          parsedOptions.resizeQuality));
    else
      m_image = StaticBitmapImage::create(skImage);
    if (!m_image)
      return;
    m_image->setPremultiplied(parsedOptions.premultiplyAlpha);
    return;
  }

  std::unique_ptr<ImageBuffer> buffer = ImageBuffer::create(
      parsedOptions.cropRect.size(), NonOpaque, DoNotInitializeImagePixels);
  if (!buffer)
    return;

  if (srcRect.isEmpty()) {
    m_image = StaticBitmapImage::create(buffer->newSkImageSnapshot(
        PreferNoAcceleration, SnapshotReasonUnknown));
    return;
  }

  IntPoint dstPoint = IntPoint(std::min(0, -parsedOptions.cropRect.x()),
                               std::min(0, -parsedOptions.cropRect.y()));
  if (parsedOptions.cropRect.x() < 0)
    dstPoint.setX(-parsedOptions.cropRect.x());
  if (parsedOptions.cropRect.y() < 0)
    dstPoint.setY(-parsedOptions.cropRect.y());
  buffer->putByteArray(Unmultiplied, data->data()->data(), data->size(),
                       srcRect, dstPoint);
  sk_sp<SkImage> skImage =
      buffer->newSkImageSnapshot(PreferNoAcceleration, SnapshotReasonUnknown);
  if (parsedOptions.flipY)
    skImage = flipSkImageVertically(skImage.get(), PremultiplyAlpha);
  if (!skImage)
    return;
  if (parsedOptions.shouldScaleInput) {
    sk_sp<SkSurface> surface = SkSurface::MakeRasterN32Premul(
        parsedOptions.resizeWidth, parsedOptions.resizeHeight);
    if (!surface)
      return;
    SkPaint paint;
    paint.setFilterQuality(parsedOptions.resizeQuality);
    SkRect dstDrawRect =
        SkRect::MakeWH(parsedOptions.resizeWidth, parsedOptions.resizeHeight);
    surface->getCanvas()->drawImageRect(skImage, dstDrawRect, &paint);
    skImage = surface->makeImageSnapshot();
  }
  m_image = StaticBitmapImage::create(std::move(skImage));
}
