static PassRefPtr<StaticBitmapImage> cropImage(
    Image* image,
    const ParsedOptions& parsedOptions,
    AlphaDisposition imageFormat = PremultiplyAlpha,
    ImageDecoder::ColorSpaceOption colorSpaceOp =
        ImageDecoder::ColorSpaceApplied) {
  ASSERT(image);
  IntRect imgRect(IntPoint(), IntSize(image->width(), image->height()));
  const IntRect srcRect = intersection(imgRect, parsedOptions.cropRect);

  if (srcRect.isEmpty() && !parsedOptions.premultiplyAlpha) {
    SkImageInfo info =
         SkImageInfo::Make(parsedOptions.resizeWidth, parsedOptions.resizeHeight,
                           kN32_SkColorType, kUnpremul_SkAlphaType);
     RefPtr<ArrayBuffer> dstBuffer = ArrayBuffer::createOrNull(
        static_cast<size_t>(info.width()) * info.height(),
         info.bytesPerPixel());
     if (!dstBuffer)
       return nullptr;
     RefPtr<Uint8Array> dstPixels =
         Uint8Array::create(dstBuffer, 0, dstBuffer->byteLength());
     return StaticBitmapImage::create(newSkImageFromRaster(
         info, std::move(dstPixels),
        static_cast<size_t>(info.width()) * info.bytesPerPixel()));
   }
 
   sk_sp<SkImage> skiaImage = image->imageForCurrentFrame();
  if ((((!parsedOptions.premultiplyAlpha && !skiaImage->isOpaque()) ||
        !skiaImage) &&
       image->data() && imageFormat == PremultiplyAlpha) ||
      colorSpaceOp == ImageDecoder::ColorSpaceIgnored) {
    std::unique_ptr<ImageDecoder> decoder(ImageDecoder::create(
        image->data(), true,
        parsedOptions.premultiplyAlpha ? ImageDecoder::AlphaPremultiplied
                                       : ImageDecoder::AlphaNotPremultiplied,
        colorSpaceOp));
    if (!decoder)
      return nullptr;
    skiaImage = ImageBitmap::getSkImageFromDecoder(std::move(decoder));
    if (!skiaImage)
      return nullptr;
  }

  if (parsedOptions.cropRect == srcRect && !parsedOptions.shouldScaleInput) {
    sk_sp<SkImage> croppedSkImage = skiaImage->makeSubset(srcRect);
    if (parsedOptions.flipY)
      return StaticBitmapImage::create(flipSkImageVertically(
          croppedSkImage.get(), parsedOptions.premultiplyAlpha
                                    ? PremultiplyAlpha
                                    : DontPremultiplyAlpha));
    if (parsedOptions.premultiplyAlpha && imageFormat == DontPremultiplyAlpha)
      return StaticBitmapImage::create(
          unPremulSkImageToPremul(croppedSkImage.get()));
    croppedSkImage->preroll();
    return StaticBitmapImage::create(std::move(croppedSkImage));
  }

  sk_sp<SkSurface> surface = SkSurface::MakeRasterN32Premul(
      parsedOptions.resizeWidth, parsedOptions.resizeHeight);
  if (!surface)
    return nullptr;
  if (srcRect.isEmpty())
    return StaticBitmapImage::create(surface->makeImageSnapshot());

  SkScalar dstLeft = std::min(0, -parsedOptions.cropRect.x());
  SkScalar dstTop = std::min(0, -parsedOptions.cropRect.y());
  if (parsedOptions.cropRect.x() < 0)
    dstLeft = -parsedOptions.cropRect.x();
  if (parsedOptions.cropRect.y() < 0)
    dstTop = -parsedOptions.cropRect.y();
  if (parsedOptions.flipY) {
    surface->getCanvas()->translate(0, surface->height());
    surface->getCanvas()->scale(1, -1);
  }
  if (parsedOptions.shouldScaleInput) {
    SkRect drawSrcRect = SkRect::MakeXYWH(
        parsedOptions.cropRect.x(), parsedOptions.cropRect.y(),
        parsedOptions.cropRect.width(), parsedOptions.cropRect.height());
    SkRect drawDstRect = SkRect::MakeXYWH(0, 0, parsedOptions.resizeWidth,
                                          parsedOptions.resizeHeight);
    SkPaint paint;
    paint.setFilterQuality(parsedOptions.resizeQuality);
    surface->getCanvas()->drawImageRect(skiaImage, drawSrcRect, drawDstRect,
                                        &paint);
  } else {
    surface->getCanvas()->drawImage(skiaImage, dstLeft, dstTop);
  }
  skiaImage = surface->makeImageSnapshot();

  if (parsedOptions.premultiplyAlpha) {
    if (imageFormat == DontPremultiplyAlpha)
      return StaticBitmapImage::create(
          unPremulSkImageToPremul(skiaImage.get()));
    return StaticBitmapImage::create(std::move(skiaImage));
  }
  return StaticBitmapImage::create(premulSkImageToUnPremul(skiaImage.get()));
}
