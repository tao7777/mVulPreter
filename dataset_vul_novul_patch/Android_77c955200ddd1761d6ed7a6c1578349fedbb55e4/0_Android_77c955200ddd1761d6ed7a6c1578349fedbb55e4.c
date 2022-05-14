SkCodec* SkIcoCodec::NewFromStream(SkStream* stream, Result* result) {
    std::unique_ptr<SkStream> inputStream(stream);

 static const uint32_t kIcoDirectoryBytes = 6;
 static const uint32_t kIcoDirEntryBytes = 16;

    std::unique_ptr<uint8_t[]> dirBuffer(new uint8_t[kIcoDirectoryBytes]);
 if (inputStream.get()->read(dirBuffer.get(), kIcoDirectoryBytes) !=
            kIcoDirectoryBytes) {
 SkCodecPrintf("Error: unable to read ico directory header.\n");
 *result = kIncompleteInput;
 return nullptr;
 }

 const uint16_t numImages = get_short(dirBuffer.get(), 4);
 if (0 == numImages) {
 SkCodecPrintf("Error: No images embedded in ico.\n");
 *result = kInvalidInput;
 return nullptr;
 }

 struct Entry {

         uint32_t offset;
         uint32_t size;
     };
    SkAutoFree dirEntryBuffer(sk_malloc_canfail(sizeof(Entry) * numImages));
     if (!dirEntryBuffer) {
         SkCodecPrintf("Error: OOM allocating ICO directory for %i images.\n",
                       numImages);
 *result = kInternalError;
 return nullptr;
 }
 auto* directoryEntries = reinterpret_cast<Entry*>(dirEntryBuffer.get());

 for (uint32_t i = 0; i < numImages; i++) {
 uint8_t entryBuffer[kIcoDirEntryBytes];
 if (inputStream->read(entryBuffer, kIcoDirEntryBytes) !=
                kIcoDirEntryBytes) {
 SkCodecPrintf("Error: Dir entries truncated in ico.\n");
 *result = kIncompleteInput;
 return nullptr;
 }


 uint32_t size = get_int(entryBuffer, 8);

 uint32_t offset = get_int(entryBuffer, 12);

        directoryEntries[i].offset = offset;
        directoryEntries[i].size = size;
 }

 *result = kInvalidInput;

 struct EntryLessThan {
 bool operator() (Entry a, Entry b) const {
 return a.offset < b.offset;
 }
 };
 EntryLessThan lessThan;
 SkTQSort(directoryEntries, &directoryEntries[numImages - 1], lessThan);

 uint32_t bytesRead = kIcoDirectoryBytes + numImages * kIcoDirEntryBytes;
    std::unique_ptr<SkTArray<std::unique_ptr<SkCodec>, true>> codecs(
 new (SkTArray<std::unique_ptr<SkCodec>, true>)(numImages));
 for (uint32_t i = 0; i < numImages; i++) {
 uint32_t offset = directoryEntries[i].offset;
 uint32_t size = directoryEntries[i].size;

 if (offset < bytesRead) {
 SkCodecPrintf("Warning: invalid ico offset.\n");
 continue;
 }

 if (inputStream.get()->skip(offset - bytesRead) != offset - bytesRead) {
 SkCodecPrintf("Warning: could not skip to ico offset.\n");
 break;
 }

         bytesRead = offset;
 
        SkAutoFree buffer(sk_malloc_canfail(size));
         if (!buffer) {
             SkCodecPrintf("Warning: OOM trying to create embedded stream.\n");
             break;
 }

 if (inputStream->read(buffer.get(), size) != size) {
 SkCodecPrintf("Warning: could not create embedded stream.\n");
 *result = kIncompleteInput;
 break;
 }

        sk_sp<SkData> data(SkData::MakeFromMalloc(buffer.release(), size));
        std::unique_ptr<SkMemoryStream> embeddedStream(new SkMemoryStream(data));
        bytesRead += size;

 SkCodec* codec = nullptr;
 Result dummyResult;
 if (SkPngCodec::IsPng((const char*) data->bytes(), data->size())) {
            codec = SkPngCodec::NewFromStream(embeddedStream.release(), &dummyResult);
 } else {
            codec = SkBmpCodec::NewFromIco(embeddedStream.release(), &dummyResult);
 }

 if (nullptr != codec) {
            codecs->push_back().reset(codec);
 }
 }

 if (0 == codecs->count()) {
 SkCodecPrintf("Error: could not find any valid embedded ico codecs.\n");
 return nullptr;
 }

 size_t maxSize = 0;
 int maxIndex = 0;
 for (int i = 0; i < codecs->count(); i++) {
 SkImageInfo info = codecs->operator[](i)->getInfo();
 size_t size = info.getSafeSize(info.minRowBytes());

 if (size > maxSize) {
            maxSize = size;
            maxIndex = i;
 }
 }
 int width = codecs->operator[](maxIndex)->getInfo().width();
 int height = codecs->operator[](maxIndex)->getInfo().height();
 SkEncodedInfo info = codecs->operator[](maxIndex)->getEncodedInfo();
 SkColorSpace* colorSpace = codecs->operator[](maxIndex)->getInfo().colorSpace();

 *result = kSuccess;
 return new SkIcoCodec(width, height, info, codecs.release(), sk_ref_sp(colorSpace));
}
