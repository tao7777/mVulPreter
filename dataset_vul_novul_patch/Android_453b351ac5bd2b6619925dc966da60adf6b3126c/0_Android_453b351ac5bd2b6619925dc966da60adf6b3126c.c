sp<VBRISeeker> VBRISeeker::CreateFromSource(
 const sp<DataSource> &source, off64_t post_id3_pos) {
 off64_t pos = post_id3_pos;

 uint8_t header[4];
 ssize_t n = source->readAt(pos, header, sizeof(header));
 if (n < (ssize_t)sizeof(header)) {
 return NULL;
 }

 uint32_t tmp = U32_AT(&header[0]);
 size_t frameSize;
 int sampleRate;
 if (!GetMPEGAudioFrameSize(tmp, &frameSize, &sampleRate)) {
 return NULL;
 }

    pos += sizeof(header) + 32;

 uint8_t vbriHeader[26];
    n = source->readAt(pos, vbriHeader, sizeof(vbriHeader));
 if (n < (ssize_t)sizeof(vbriHeader)) {
 return NULL;
 }

 if (memcmp(vbriHeader, "VBRI", 4)) {
 return NULL;
 }

 size_t numFrames = U32_AT(&vbriHeader[14]);

 int64_t durationUs =
        numFrames * 1000000ll * (sampleRate >= 32000 ? 1152 : 576) / sampleRate;

    ALOGV("duration = %.2f secs", durationUs / 1E6);

 size_t numEntries = U16_AT(&vbriHeader[18]);
 size_t entrySize = U16_AT(&vbriHeader[22]);
 size_t scale = U16_AT(&vbriHeader[20]);

    ALOGV("%zu entries, scale=%zu, size_per_entry=%zu",
         numEntries,

          scale,
          entrySize);
 
    if (entrySize > 4) {
        ALOGE("invalid VBRI entry size: %zu", entrySize);
        return NULL;
    }

    sp<VBRISeeker> seeker = new (std::nothrow) VBRISeeker;
    if (seeker == NULL) {
        ALOGW("Couldn't allocate VBRISeeker");
        return NULL;
    }

     size_t totalEntrySize = numEntries * entrySize;
    uint8_t *buffer = new (std::nothrow) uint8_t[totalEntrySize];
    if (!buffer) {
        ALOGW("Couldn't allocate %zu bytes", totalEntrySize);
        return NULL;
    }
 
     n = source->readAt(pos + sizeof(vbriHeader), buffer, totalEntrySize);
     if (n < (ssize_t)totalEntrySize) {
 delete[] buffer;
        buffer = NULL;


         return NULL;
     }
 
     seeker->mBasePos = post_id3_pos + frameSize;
 if (durationUs) {
        seeker->mDurationUs = durationUs;
 }

 off64_t offset = post_id3_pos;
 for (size_t i = 0; i < numEntries; ++i) {
 uint32_t numBytes;
 switch (entrySize) {
 case 1: numBytes = buffer[i]; break;
 case 2: numBytes = U16_AT(buffer + 2 * i); break;
 case 3: numBytes = U24_AT(buffer + 3 * i); break;
 default:
 {
                CHECK_EQ(entrySize, 4u);
                numBytes = U32_AT(buffer + 4 * i); break;
 }
 }

        numBytes *= scale;

        seeker->mSegments.push(numBytes);

        ALOGV("entry #%zu: %u offset %#016llx", i, numBytes, (long long)offset);
        offset += numBytes;
 }

 delete[] buffer;
    buffer = NULL;

    ALOGI("Found VBRI header.");

 return seeker;
}
