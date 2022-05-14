static PassRefPtr<Uint8Array> copySkImageData(SkImage* input,
                                               const SkImageInfo& info) {
  // width * height * bytesPerPixel will never overflow unsigned.
  unsigned width = static_cast<unsigned>(input->width());
   RefPtr<ArrayBuffer> dstBuffer =
       ArrayBuffer::createOrNull(width * input->height(), info.bytesPerPixel());
   if (!dstBuffer)
    return nullptr;
  RefPtr<Uint8Array> dstPixels =
      Uint8Array::create(dstBuffer, 0, dstBuffer->byteLength());
  input->readPixels(info, dstPixels->data(), width * info.bytesPerPixel(), 0,
                    0);
  return dstPixels;
}
