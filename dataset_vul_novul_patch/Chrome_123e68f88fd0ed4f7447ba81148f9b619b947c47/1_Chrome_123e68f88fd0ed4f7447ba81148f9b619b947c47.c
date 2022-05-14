File* DataObjectItem::GetAsFile() const {
  if (Kind() != kFileKind)
    return nullptr;

  if (source_ == kInternalSource) {
    if (file_)
      return file_.Get();
    DCHECK(shared_buffer_);
    return nullptr;
  }

  DCHECK_EQ(source_, kClipboardSource);
  if (GetType() == kMimeTypeImagePng) {
    SkBitmap bitmap = SystemClipboard::GetInstance().ReadImage(
        mojom::ClipboardBuffer::kStandard);

     SkPixmap pixmap;
     bitmap.peekPixels(&pixmap);
 
    Vector<uint8_t> png_data;
     SkPngEncoder::Options options;
    options.fZLibLevel = 1;  // Fastest compression.
     if (!ImageEncoder::Encode(&png_data, pixmap, options))
       return nullptr;
 
    auto data = std::make_unique<BlobData>();
    data->SetContentType(kMimeTypeImagePng);
    data->AppendBytes(png_data.data(), png_data.size());
    const uint64_t length = data->length();
    auto blob = BlobDataHandle::Create(std::move(data), length);
    return File::Create("image.png", base::Time::Now().ToDoubleT() * 1000.0,
                        std::move(blob));
  }

  return nullptr;
}
