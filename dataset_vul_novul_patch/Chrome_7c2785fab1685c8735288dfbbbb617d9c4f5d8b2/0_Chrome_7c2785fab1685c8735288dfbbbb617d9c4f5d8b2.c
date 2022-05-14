bool GraphicsContext3D::getImageData(Image* image,
                                     GC3Denum format,
                                     GC3Denum type,
                                     bool premultiplyAlpha,
                                     bool ignoreGammaAndColorProfile,
                                     Vector<uint8_t>& outputVector)
{
    if (!image)
         return false;
     CGImageRef cgImage;
     RetainPtr<CGImageRef> decodedImage;
    bool hasAlpha = image->isBitmapImage() ? image->currentFrameHasAlpha() : true;
     if ((ignoreGammaAndColorProfile || (hasAlpha && !premultiplyAlpha)) && image->data()) {
         ImageSource decoder(ImageSource::AlphaNotPremultiplied,
                             ignoreGammaAndColorProfile ? ImageSource::GammaAndColorProfileIgnored : ImageSource::GammaAndColorProfileApplied);
        decoder.setData(image->data(), true);
        if (!decoder.frameCount())
            return false;
        decodedImage.adoptCF(decoder.createFrameAtIndex(0));
        cgImage = decodedImage.get();
    } else
        cgImage = image->nativeImageForCurrentFrame();
    if (!cgImage)
        return false;

    size_t width = CGImageGetWidth(cgImage);
    size_t height = CGImageGetHeight(cgImage);
    if (!width || !height)
        return false;

    CGColorSpaceRef colorSpace = CGImageGetColorSpace(cgImage);
    CGColorSpaceModel model = CGColorSpaceGetModel(colorSpace);
    if (model == kCGColorSpaceModelIndexed) {
        RetainPtr<CGContextRef> bitmapContext;
        bitmapContext.adoptCF(CGBitmapContextCreate(0, width, height, 8, width * 4,
                                                    deviceRGBColorSpaceRef(),
                                                    kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host));
        if (!bitmapContext)
            return false;

        CGContextSetBlendMode(bitmapContext.get(), kCGBlendModeCopy);
        CGContextSetInterpolationQuality(bitmapContext.get(), kCGInterpolationNone);
        CGContextDrawImage(bitmapContext.get(), CGRectMake(0, 0, width, height), cgImage);

        decodedImage.adoptCF(CGBitmapContextCreateImage(bitmapContext.get()));
        cgImage = decodedImage.get();
    }

    size_t bitsPerComponent = CGImageGetBitsPerComponent(cgImage);
    size_t bitsPerPixel = CGImageGetBitsPerPixel(cgImage);
    if (bitsPerComponent != 8 && bitsPerComponent != 16)
        return false;
    if (bitsPerPixel % bitsPerComponent)
        return false;
    size_t componentsPerPixel = bitsPerPixel / bitsPerComponent;

    CGBitmapInfo bitInfo = CGImageGetBitmapInfo(cgImage);
    bool bigEndianSource = false;
    if (bitsPerComponent == 16) {
        switch (bitInfo & kCGBitmapByteOrderMask) {
        case kCGBitmapByteOrder16Big:
            bigEndianSource = true;
            break;
        case kCGBitmapByteOrder16Little:
            bigEndianSource = false;
            break;
        case kCGBitmapByteOrderDefault:
            bigEndianSource = true;
            break;
        default:
            return false;
        }
    } else {
        switch (bitInfo & kCGBitmapByteOrderMask) {
        case kCGBitmapByteOrder32Big:
            bigEndianSource = true;
            break;
        case kCGBitmapByteOrder32Little:
            bigEndianSource = false;
            break;
        case kCGBitmapByteOrderDefault:
            bigEndianSource = true;
            break;
        default:
            return false;
        }
    }

    AlphaOp neededAlphaOp = AlphaDoNothing;
    AlphaFormat alphaFormat = AlphaFormatNone;
    switch (CGImageGetAlphaInfo(cgImage)) {
    case kCGImageAlphaPremultipliedFirst:
        if (!premultiplyAlpha)
            neededAlphaOp = AlphaDoUnmultiply;
        alphaFormat = AlphaFormatFirst;
        break;
    case kCGImageAlphaFirst:
        if (premultiplyAlpha)
            neededAlphaOp = AlphaDoPremultiply;
        alphaFormat = AlphaFormatFirst;
        break;
    case kCGImageAlphaNoneSkipFirst:
        alphaFormat = AlphaFormatFirst;
        break;
    case kCGImageAlphaPremultipliedLast:
        if (!premultiplyAlpha)
            neededAlphaOp = AlphaDoUnmultiply;
        alphaFormat = AlphaFormatLast;
        break;
    case kCGImageAlphaLast:
        if (premultiplyAlpha)
            neededAlphaOp = AlphaDoPremultiply;
        alphaFormat = AlphaFormatLast;
        break;
    case kCGImageAlphaNoneSkipLast:
        alphaFormat = AlphaFormatLast;
        break;
    case kCGImageAlphaNone:
        alphaFormat = AlphaFormatNone;
        break;
    default:
        return false;
    }
    SourceDataFormat srcDataFormat = getSourceDataFormat(componentsPerPixel, alphaFormat, bitsPerComponent == 16, bigEndianSource);
    if (srcDataFormat == SourceFormatNumFormats)
        return false;

    RetainPtr<CFDataRef> pixelData;
    pixelData.adoptCF(CGDataProviderCopyData(CGImageGetDataProvider(cgImage)));
    if (!pixelData)
        return false;
    const UInt8* rgba = CFDataGetBytePtr(pixelData.get());

    unsigned int packedSize;
    if (computeImageSizeInBytes(format, type, width, height, 1, &packedSize, 0) != GraphicsContext3D::NO_ERROR)
        return false;
    outputVector.resize(packedSize);

    unsigned int srcUnpackAlignment = 0;
    size_t bytesPerRow = CGImageGetBytesPerRow(cgImage);
    unsigned int padding = bytesPerRow - bitsPerPixel / 8 * width;
    if (padding) {
        srcUnpackAlignment = padding + 1;
        while (bytesPerRow % srcUnpackAlignment)
            ++srcUnpackAlignment;
    }
    bool rt = packPixels(rgba, srcDataFormat, width, height, srcUnpackAlignment,
                         format, type, neededAlphaOp, outputVector.data());
    return rt;
}
