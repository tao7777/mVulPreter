status_t MPEG4Extractor::parseChunk(off64_t *offset, int depth) {
    ALOGV("entering parseChunk %lld/%d", *offset, depth);
 uint32_t hdr[2];
 if (mDataSource->readAt(*offset, hdr, 8) < 8) {
 return ERROR_IO;
 }
 uint64_t chunk_size = ntohl(hdr[0]);
 uint32_t chunk_type = ntohl(hdr[1]);
 off64_t data_offset = *offset + 8;

 if (chunk_size == 1) {
 if (mDataSource->readAt(*offset + 8, &chunk_size, 8) < 8) {
 return ERROR_IO;
 }
        chunk_size = ntoh64(chunk_size);
        data_offset += 8;

 if (chunk_size < 16) {
 return ERROR_MALFORMED;
 }
 } else if (chunk_size == 0) {
 if (depth == 0) {
 off64_t sourceSize;
 if (mDataSource->getSize(&sourceSize) == OK) {
                chunk_size = (sourceSize - *offset);
 } else {
                ALOGE("atom size is 0, and data source has no size");
 return ERROR_MALFORMED;
 }
 } else {
 *offset += 4;
 return OK;
 }
 } else if (chunk_size < 8) {
        ALOGE("invalid chunk size: %" PRIu64, chunk_size);
 return ERROR_MALFORMED;
 }

 char chunk[5];
 MakeFourCCString(chunk_type, chunk);
    ALOGV("chunk: %s @ %lld, %d", chunk, *offset, depth);

#if 0
 static const char kWhitespace[] = "                                        ";
 const char *indent = &kWhitespace[sizeof(kWhitespace) - 1 - 2 * depth];
    printf("%sfound chunk '%s' of size %" PRIu64 "\n", indent, chunk, chunk_size);

 char buffer[256];
 size_t n = chunk_size;
 if (n > sizeof(buffer)) {
        n = sizeof(buffer);
 }
 if (mDataSource->readAt(*offset, buffer, n)
 < (ssize_t)n) {
 return ERROR_IO;
 }

    hexdump(buffer, n);
#endif

 PathAdder autoAdder(&mPath, chunk_type);

 off64_t chunk_data_size = *offset + chunk_size - data_offset;

 if (chunk_type != FOURCC('c', 'p', 'r', 't')
 && chunk_type != FOURCC('c', 'o', 'v', 'r')
 && mPath.size() == 5 && underMetaDataPath(mPath)) {
 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset;
 while (*offset < stop_offset) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }

 return OK;
 }

 switch(chunk_type) {
 case FOURCC('m', 'o', 'o', 'v'):
 case FOURCC('t', 'r', 'a', 'k'):
 case FOURCC('m', 'd', 'i', 'a'):
 case FOURCC('m', 'i', 'n', 'f'):
 case FOURCC('d', 'i', 'n', 'f'):
 case FOURCC('s', 't', 'b', 'l'):
 case FOURCC('m', 'v', 'e', 'x'):
 case FOURCC('m', 'o', 'o', 'f'):
 case FOURCC('t', 'r', 'a', 'f'):
 case FOURCC('m', 'f', 'r', 'a'):
 case FOURCC('u', 'd', 't', 'a'):
 case FOURCC('i', 'l', 's', 't'):
 case FOURCC('s', 'i', 'n', 'f'):
 case FOURCC('s', 'c', 'h', 'i'):
 case FOURCC('e', 'd', 't', 's'):
 {
 if (chunk_type == FOURCC('s', 't', 'b', 'l')) {
                ALOGV("sampleTable chunk is %" PRIu64 " bytes long.", chunk_size);

 if (mDataSource->flags()
 & (DataSource::kWantsPrefetching
 | DataSource::kIsCachingDataSource)) {
                    sp<MPEG4DataSource> cachedSource =
 new MPEG4DataSource(mDataSource);

 if (cachedSource->setCachedRange(*offset, chunk_size) == OK) {
                        mDataSource = cachedSource;
 }
 }

                mLastTrack->sampleTable = new SampleTable(mDataSource);
 }

 bool isTrack = false;
 if (chunk_type == FOURCC('t', 'r', 'a', 'k')) {
                isTrack = true;

 Track *track = new Track;
                track->next = NULL;
 if (mLastTrack) {
                    mLastTrack->next = track;
 } else {
                    mFirstTrack = track;
 }
                mLastTrack = track;

                track->meta = new MetaData;
                track->includes_expensive_metadata = false;
                track->skipTrack = false;
                track->timescale = 0;
                track->meta->setCString(kKeyMIMEType, "application/octet-stream");
 }

 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset;
 while (*offset < stop_offset) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }

 if (isTrack) {
 if (mLastTrack->skipTrack) {
 Track *cur = mFirstTrack;

 if (cur == mLastTrack) {
 delete cur;
                        mFirstTrack = mLastTrack = NULL;
 } else {
 while (cur && cur->next != mLastTrack) {
                            cur = cur->next;
 }
                        cur->next = NULL;
 delete mLastTrack;
                        mLastTrack = cur;
 }

 return OK;
 }

 status_t err = verifyTrack(mLastTrack);

 if (err != OK) {
 return err;
 }
 } else if (chunk_type == FOURCC('m', 'o', 'o', 'v')) {
                mInitCheck = OK;

 if (!mIsDrm) {
 return UNKNOWN_ERROR; // Return a dummy error.
 } else {
 return OK;
 }
 }
 break;
 }

 case FOURCC('e', 'l', 's', 't'):
 {
 *offset += chunk_size;

 uint8_t version;
 if (mDataSource->readAt(data_offset, &version, 1) < 1) {
 return ERROR_IO;
 }

 uint32_t entry_count;
 if (!mDataSource->getUInt32(data_offset + 4, &entry_count)) {
 return ERROR_IO;
 }

 if (entry_count != 1) {
                ALOGW("ignoring edit list with %d entries", entry_count);
 } else if (mHeaderTimescale == 0) {
                ALOGW("ignoring edit list because timescale is 0");
 } else {
 off64_t entriesoffset = data_offset + 8;
 uint64_t segment_duration;
 int64_t media_time;

 if (version == 1) {
 if (!mDataSource->getUInt64(entriesoffset, &segment_duration) ||
 !mDataSource->getUInt64(entriesoffset + 8, (uint64_t*)&media_time)) {
 return ERROR_IO;
 }
 } else if (version == 0) {
 uint32_t sd;
 int32_t mt;
 if (!mDataSource->getUInt32(entriesoffset, &sd) ||
 !mDataSource->getUInt32(entriesoffset + 4, (uint32_t*)&mt)) {
 return ERROR_IO;
 }
                    segment_duration = sd;
                    media_time = mt;
 } else {
 return ERROR_IO;
 }

 uint64_t halfscale = mHeaderTimescale / 2;
                segment_duration = (segment_duration * 1000000 + halfscale)/ mHeaderTimescale;
                media_time = (media_time * 1000000 + halfscale) / mHeaderTimescale;

 int64_t duration;
 int32_t samplerate;
 if (!mLastTrack) {
 return ERROR_MALFORMED;
 }
 if (mLastTrack->meta->findInt64(kKeyDuration, &duration) &&
                        mLastTrack->meta->findInt32(kKeySampleRate, &samplerate)) {

 int64_t delay = (media_time  * samplerate + 500000) / 1000000;
                    mLastTrack->meta->setInt32(kKeyEncoderDelay, delay);

 int64_t paddingus = duration - (segment_duration + media_time);
 if (paddingus < 0) {
                        paddingus = 0;
 }
 int64_t paddingsamples = (paddingus * samplerate + 500000) / 1000000;
                    mLastTrack->meta->setInt32(kKeyEncoderPadding, paddingsamples);
 }
 }
 break;
 }

 case FOURCC('f', 'r', 'm', 'a'):
 {
 *offset += chunk_size;

 uint32_t original_fourcc;
 if (mDataSource->readAt(data_offset, &original_fourcc, 4) < 4) {
 return ERROR_IO;
 }
            original_fourcc = ntohl(original_fourcc);
            ALOGV("read original format: %d", original_fourcc);
            mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(original_fourcc));
 uint32_t num_channels = 0;
 uint32_t sample_rate = 0;
 if (AdjustChannelsAndRate(original_fourcc, &num_channels, &sample_rate)) {
                mLastTrack->meta->setInt32(kKeyChannelCount, num_channels);
                mLastTrack->meta->setInt32(kKeySampleRate, sample_rate);
 }
 break;
 }

 case FOURCC('t', 'e', 'n', 'c'):
 {
 *offset += chunk_size;

 if (chunk_size < 32) {
 return ERROR_MALFORMED;
 }

 char buf[4];
            memset(buf, 0, 4);
 if (mDataSource->readAt(data_offset + 4, buf + 1, 3) < 3) {
 return ERROR_IO;
 }
 uint32_t defaultAlgorithmId = ntohl(*((int32_t*)buf));
 if (defaultAlgorithmId > 1) {
 return ERROR_MALFORMED;
 }

            memset(buf, 0, 4);
 if (mDataSource->readAt(data_offset + 7, buf + 3, 1) < 1) {
 return ERROR_IO;
 }
 uint32_t defaultIVSize = ntohl(*((int32_t*)buf));

 if ((defaultAlgorithmId == 0 && defaultIVSize != 0) ||
 (defaultAlgorithmId != 0 && defaultIVSize == 0)) {
 return ERROR_MALFORMED;
 } else if (defaultIVSize != 0 &&
                    defaultIVSize != 8 &&
                    defaultIVSize != 16) {
 return ERROR_MALFORMED;
 }

 uint8_t defaultKeyId[16];

 if (mDataSource->readAt(data_offset + 8, &defaultKeyId, 16) < 16) {
 return ERROR_IO;
 }

            mLastTrack->meta->setInt32(kKeyCryptoMode, defaultAlgorithmId);
            mLastTrack->meta->setInt32(kKeyCryptoDefaultIVSize, defaultIVSize);
            mLastTrack->meta->setData(kKeyCryptoKey, 'tenc', defaultKeyId, 16);
 break;
 }

 case FOURCC('t', 'k', 'h', 'd'):
 {
 *offset += chunk_size;

 status_t err;
 if ((err = parseTrackHeader(data_offset, chunk_data_size)) != OK) {
 return err;
 }

 break;
 }

 case FOURCC('p', 's', 's', 'h'):
 {
 *offset += chunk_size;

 PsshInfo pssh;

 if (mDataSource->readAt(data_offset + 4, &pssh.uuid, 16) < 16) {
 return ERROR_IO;
 }

 uint32_t psshdatalen = 0;
 if (mDataSource->readAt(data_offset + 20, &psshdatalen, 4) < 4) {
 return ERROR_IO;
 }
            pssh.datalen = ntohl(psshdatalen);
            ALOGV("pssh data size: %d", pssh.datalen);
 if (pssh.datalen + 20 > chunk_size) {
 return ERROR_MALFORMED;
 }

            pssh.data = new (std::nothrow) uint8_t[pssh.datalen];
 if (pssh.data == NULL) {
 return ERROR_MALFORMED;
 }
            ALOGV("allocated pssh @ %p", pssh.data);
 ssize_t requested = (ssize_t) pssh.datalen;
 if (mDataSource->readAt(data_offset + 24, pssh.data, requested) < requested) {
 return ERROR_IO;
 }
            mPssh.push_back(pssh);

 break;
 }

 case FOURCC('m', 'd', 'h', 'd'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 4 || mLastTrack == NULL) {
 return ERROR_MALFORMED;
 }

 uint8_t version;
 if (mDataSource->readAt(
                        data_offset, &version, sizeof(version))
 < (ssize_t)sizeof(version)) {
 return ERROR_IO;
 }

 off64_t timescale_offset;

 if (version == 1) {
                timescale_offset = data_offset + 4 + 16;
 } else if (version == 0) {
                timescale_offset = data_offset + 4 + 8;
 } else {
 return ERROR_IO;
 }

 uint32_t timescale;
 if (mDataSource->readAt(
                        timescale_offset, &timescale, sizeof(timescale))
 < (ssize_t)sizeof(timescale)) {
 return ERROR_IO;
 }

            mLastTrack->timescale = ntohl(timescale);

 int64_t duration = 0;
 if (version == 1) {
 if (mDataSource->readAt(
                            timescale_offset + 4, &duration, sizeof(duration))
 < (ssize_t)sizeof(duration)) {
 return ERROR_IO;
 }
 if (duration != -1) {
                    duration = ntoh64(duration);
 }
 } else {
 uint32_t duration32;
 if (mDataSource->readAt(
                            timescale_offset + 4, &duration32, sizeof(duration32))
 < (ssize_t)sizeof(duration32)) {
 return ERROR_IO;
 }
 if (duration32 != 0xffffffff) {
                    duration = ntohl(duration32);
 }
 }
 if (duration != 0) {
                mLastTrack->meta->setInt64(
                        kKeyDuration, (duration * 1000000) / mLastTrack->timescale);
 }

 uint8_t lang[2];
 off64_t lang_offset;
 if (version == 1) {
                lang_offset = timescale_offset + 4 + 8;
 } else if (version == 0) {
                lang_offset = timescale_offset + 4 + 4;
 } else {
 return ERROR_IO;
 }

 if (mDataSource->readAt(lang_offset, &lang, sizeof(lang))
 < (ssize_t)sizeof(lang)) {
 return ERROR_IO;
 }

 char lang_code[4];
            lang_code[0] = ((lang[0] >> 2) & 0x1f) + 0x60;
            lang_code[1] = ((lang[0] & 0x3) << 3 | (lang[1] >> 5)) + 0x60;
            lang_code[2] = (lang[1] & 0x1f) + 0x60;
            lang_code[3] = '\0';

            mLastTrack->meta->setCString(
                    kKeyMediaLanguage, lang_code);

 break;
 }

 case FOURCC('s', 't', 's', 'd'):
 {
 if (chunk_data_size < 8) {
 return ERROR_MALFORMED;
 }

 uint8_t buffer[8];
 if (chunk_data_size < (off64_t)sizeof(buffer)) {
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                        data_offset, buffer, 8) < 8) {
 return ERROR_IO;
 }

 if (U32_AT(buffer) != 0) {
 return ERROR_MALFORMED;
 }

 uint32_t entry_count = U32_AT(&buffer[4]);

 if (entry_count > 1) {
 const char *mime;
                CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
 if (strcasecmp(mime, MEDIA_MIMETYPE_TEXT_3GPP) &&
                        strcasecmp(mime, "application/octet-stream")) {
                    mLastTrack->skipTrack = true;
 *offset += chunk_size;
 break;
 }
 }
 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset + 8;
 for (uint32_t i = 0; i < entry_count; ++i) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }
 break;
 }

 case FOURCC('m', 'p', '4', 'a'):
 case FOURCC('e', 'n', 'c', 'a'):
 case FOURCC('s', 'a', 'm', 'r'):
 case FOURCC('s', 'a', 'w', 'b'):
 {
 uint8_t buffer[8 + 20];
 if (chunk_data_size < (ssize_t)sizeof(buffer)) {
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
 return ERROR_IO;
 }

 uint16_t data_ref_index = U16_AT(&buffer[6]);
 uint32_t num_channels = U16_AT(&buffer[16]);

 uint16_t sample_size = U16_AT(&buffer[18]);
 uint32_t sample_rate = U32_AT(&buffer[24]) >> 16;

 if (chunk_type != FOURCC('e', 'n', 'c', 'a')) {
                mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(chunk_type));
 AdjustChannelsAndRate(chunk_type, &num_channels, &sample_rate);
 }
            ALOGV("*** coding='%s' %d channels, size %d, rate %d\n",
                   chunk, num_channels, sample_size, sample_rate);
            mLastTrack->meta->setInt32(kKeyChannelCount, num_channels);
            mLastTrack->meta->setInt32(kKeySampleRate, sample_rate);

 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset + sizeof(buffer);
 while (*offset < stop_offset) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }
 break;
 }

 case FOURCC('m', 'p', '4', 'v'):
 case FOURCC('e', 'n', 'c', 'v'):
 case FOURCC('s', '2', '6', '3'):
 case FOURCC('H', '2', '6', '3'):
 case FOURCC('h', '2', '6', '3'):
 case FOURCC('a', 'v', 'c', '1'):
 case FOURCC('h', 'v', 'c', '1'):
 case FOURCC('h', 'e', 'v', '1'):
 {
            mHasVideo = true;

 uint8_t buffer[78];
 if (chunk_data_size < (ssize_t)sizeof(buffer)) {
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
 return ERROR_IO;
 }

 uint16_t data_ref_index = U16_AT(&buffer[6]);
 uint16_t width = U16_AT(&buffer[6 + 18]);
 uint16_t height = U16_AT(&buffer[6 + 20]);

 if (width == 0)  width  = 352;
 if (height == 0) height = 288;


 if (chunk_type != FOURCC('e', 'n', 'c', 'v')) {
                mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(chunk_type));
 }
            mLastTrack->meta->setInt32(kKeyWidth, width);
            mLastTrack->meta->setInt32(kKeyHeight, height);

 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset + sizeof(buffer);
 while (*offset < stop_offset) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }
 break;
 }

 case FOURCC('s', 't', 'c', 'o'):
 case FOURCC('c', 'o', '6', '4'):
 {
 status_t err =
                mLastTrack->sampleTable->setChunkOffsetParams(
                        chunk_type, data_offset, chunk_data_size);

 *offset += chunk_size;

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('s', 't', 's', 'c'):
 {
 status_t err =
                mLastTrack->sampleTable->setSampleToChunkParams(
                        data_offset, chunk_data_size);

 *offset += chunk_size;

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('s', 't', 's', 'z'):
 case FOURCC('s', 't', 'z', '2'):
 {
 status_t err =
                mLastTrack->sampleTable->setSampleSizeParams(
                        chunk_type, data_offset, chunk_data_size);

 *offset += chunk_size;

 if (err != OK) {
 return err;
 }

 size_t max_size;
            err = mLastTrack->sampleTable->getMaxSampleSize(&max_size);

 if (err != OK) {
 return err;
 }

 if (max_size != 0) {
                mLastTrack->meta->setInt32(kKeyMaxInputSize, max_size + 10 * 2);
 } else {
 int32_t width, height;
 if (!mLastTrack->meta->findInt32(kKeyWidth, &width) ||
 !mLastTrack->meta->findInt32(kKeyHeight, &height)) {
                    ALOGE("No width or height, assuming worst case 1080p");
                    width = 1920;
                    height = 1080;
 }

 const char *mime;
                CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
 if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) {
                    max_size = ((width + 15) / 16) * ((height + 15) / 16) * 192;
 } else {
                    max_size = width * height * 3 / 2;
 }
                mLastTrack->meta->setInt32(kKeyMaxInputSize, max_size);
 }

 const char *mime;
            CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
 if (!strncasecmp("video/", mime, 6)) {
 size_t nSamples = mLastTrack->sampleTable->countSamples();
 int64_t durationUs;
 if (mLastTrack->meta->findInt64(kKeyDuration, &durationUs)) {
 if (durationUs > 0) {
 int32_t frameRate = (nSamples * 1000000LL +
 (durationUs >> 1)) / durationUs;
                        mLastTrack->meta->setInt32(kKeyFrameRate, frameRate);
 }
 }
 }

 break;
 }

 case FOURCC('s', 't', 't', 's'):
 {
 *offset += chunk_size;

 status_t err =
                mLastTrack->sampleTable->setTimeToSampleParams(
                        data_offset, chunk_data_size);

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('c', 't', 't', 's'):
 {
 *offset += chunk_size;

 status_t err =
                mLastTrack->sampleTable->setCompositionTimeToSampleParams(
                        data_offset, chunk_data_size);

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('s', 't', 's', 's'):
 {
 *offset += chunk_size;

 status_t err =
                mLastTrack->sampleTable->setSyncSampleParams(
                        data_offset, chunk_data_size);

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('\xA9', 'x', 'y', 'z'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 8) {
 return ERROR_MALFORMED;
 }

 char buffer[18];

 off64_t location_length = chunk_data_size - 5;
 if (location_length >= (off64_t) sizeof(buffer)) {
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                        data_offset + 4, buffer, location_length) < location_length) {
 return ERROR_IO;
 }

            buffer[location_length] = '\0';
            mFileMetaData->setCString(kKeyLocation, buffer);
 break;
 }

 case FOURCC('e', 's', 'd', 's'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 4) {
 return ERROR_MALFORMED;
 }

 uint8_t buffer[256];
 if (chunk_data_size > (off64_t)sizeof(buffer)) {
 return ERROR_BUFFER_TOO_SMALL;
 }

 if (mDataSource->readAt(
                        data_offset, buffer, chunk_data_size) < chunk_data_size) {
 return ERROR_IO;
 }

 if (U32_AT(buffer) != 0) {
 return ERROR_MALFORMED;
 }

            mLastTrack->meta->setData(
                    kKeyESDS, kTypeESDS, &buffer[4], chunk_data_size - 4);

 if (mPath.size() >= 2
 && mPath[mPath.size() - 2] == FOURCC('m', 'p', '4', 'a')) {

 status_t err = updateAudioTrackInfoFromESDS_MPEG4Audio(
 &buffer[4], chunk_data_size - 4);

 if (err != OK) {
 return err;
 }
 }

 break;
 }

 case FOURCC('a', 'v', 'c', 'C'):
 {
 *offset += chunk_size;

            sp<ABuffer> buffer = new ABuffer(chunk_data_size);

 if (mDataSource->readAt(
                        data_offset, buffer->data(), chunk_data_size) < chunk_data_size) {
 return ERROR_IO;
 }

            mLastTrack->meta->setData(
                    kKeyAVCC, kTypeAVCC, buffer->data(), chunk_data_size);

 break;
 }
 case FOURCC('h', 'v', 'c', 'C'):
 {
            sp<ABuffer> buffer = new ABuffer(chunk_data_size);

 if (mDataSource->readAt(
                        data_offset, buffer->data(), chunk_data_size) < chunk_data_size) {
 return ERROR_IO;
 }

            mLastTrack->meta->setData(
                    kKeyHVCC, kTypeHVCC, buffer->data(), chunk_data_size);

 *offset += chunk_size;
 break;
 }

 case FOURCC('d', '2', '6', '3'):
 {
 *offset += chunk_size;
 /*
             * d263 contains a fixed 7 bytes part:
             *   vendor - 4 bytes
             *   version - 1 byte
             *   level - 1 byte
             *   profile - 1 byte
             * optionally, "d263" box itself may contain a 16-byte
             * bit rate box (bitr)
             *   average bit rate - 4 bytes
             *   max bit rate - 4 bytes
             */
 char buffer[23];
 if (chunk_data_size != 7 &&
                chunk_data_size != 23) {
                ALOGE("Incorrect D263 box size %lld", chunk_data_size);
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                    data_offset, buffer, chunk_data_size) < chunk_data_size) {
 return ERROR_IO;
 }

            mLastTrack->meta->setData(kKeyD263, kTypeD263, buffer, chunk_data_size);

 break;
 }

 case FOURCC('m', 'e', 't', 'a'):
 {
 uint8_t buffer[4];
 if (chunk_data_size < (off64_t)sizeof(buffer)) {
 *offset += chunk_size;
 return ERROR_MALFORMED;
 }

 if (mDataSource->readAt(
                        data_offset, buffer, 4) < 4) {
 *offset += chunk_size;
 return ERROR_IO;
 }

 if (U32_AT(buffer) != 0) {

 *offset += chunk_size;
 return OK;
 }

 off64_t stop_offset = *offset + chunk_size;
 *offset = data_offset + sizeof(buffer);
 while (*offset < stop_offset) {
 status_t err = parseChunk(offset, depth + 1);
 if (err != OK) {
 return err;
 }
 }

 if (*offset != stop_offset) {
 return ERROR_MALFORMED;
 }
 break;
 }

 case FOURCC('m', 'e', 'a', 'n'):
 case FOURCC('n', 'a', 'm', 'e'):
 case FOURCC('d', 'a', 't', 'a'):
 {
 *offset += chunk_size;

 if (mPath.size() == 6 && underMetaDataPath(mPath)) {
 status_t err = parseITunesMetaData(data_offset, chunk_data_size);

 if (err != OK) {
 return err;
 }
 }

 break;
 }

 case FOURCC('m', 'v', 'h', 'd'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 32) {
 return ERROR_MALFORMED;
 }

 uint8_t header[32];
 if (mDataSource->readAt(
                        data_offset, header, sizeof(header))
 < (ssize_t)sizeof(header)) {
 return ERROR_IO;
 }

 uint64_t creationTime;
 uint64_t duration = 0;
 if (header[0] == 1) {
                creationTime = U64_AT(&header[4]);
                mHeaderTimescale = U32_AT(&header[20]);
                duration = U64_AT(&header[24]);
 if (duration == 0xffffffffffffffff) {
                    duration = 0;
 }
 } else if (header[0] != 0) {
 return ERROR_MALFORMED;
 } else {
                creationTime = U32_AT(&header[4]);
                mHeaderTimescale = U32_AT(&header[12]);
 uint32_t d32 = U32_AT(&header[16]);
 if (d32 == 0xffffffff) {
                    d32 = 0;
 }
                duration = d32;
 }
 if (duration != 0) {
                mFileMetaData->setInt64(kKeyDuration, duration * 1000000 / mHeaderTimescale);
 }

 String8 s;
            convertTimeToDate(creationTime, &s);

            mFileMetaData->setCString(kKeyDate, s.string());

 break;
 }

 case FOURCC('m', 'e', 'h', 'd'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 8) {
 return ERROR_MALFORMED;
 }

 uint8_t flags[4];
 if (mDataSource->readAt(
                        data_offset, flags, sizeof(flags))
 < (ssize_t)sizeof(flags)) {
 return ERROR_IO;
 }

 uint64_t duration = 0;
 if (flags[0] == 1) {
 if (chunk_data_size < 12) {
 return ERROR_MALFORMED;
 }
                mDataSource->getUInt64(data_offset + 4, &duration);
 if (duration == 0xffffffffffffffff) {
                    duration = 0;
 }
 } else if (flags[0] == 0) {
 uint32_t d32;
                mDataSource->getUInt32(data_offset + 4, &d32);
 if (d32 == 0xffffffff) {
                    d32 = 0;
 }
                duration = d32;
 } else {
 return ERROR_MALFORMED;
 }

 if (duration != 0) {
                mFileMetaData->setInt64(kKeyDuration, duration * 1000000 / mHeaderTimescale);
 }

 break;
 }

 case FOURCC('m', 'd', 'a', 't'):
 {
            ALOGV("mdat chunk, drm: %d", mIsDrm);
 if (!mIsDrm) {
 *offset += chunk_size;
 break;
 }

 if (chunk_size < 8) {
 return ERROR_MALFORMED;
 }

 return parseDrmSINF(offset, data_offset);
 }

 case FOURCC('h', 'd', 'l', 'r'):
 {
 *offset += chunk_size;

 uint32_t buffer;
 if (mDataSource->readAt(
                        data_offset + 8, &buffer, 4) < 4) {
 return ERROR_IO;
 }

 uint32_t type = ntohl(buffer);
 if (type == FOURCC('t', 'e', 'x', 't') || type == FOURCC('s', 'b', 't', 'l')) {
                mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_TEXT_3GPP);
 }

 break;
 }

 case FOURCC('t', 'r', 'e', 'x'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 24) {
 return ERROR_IO;
 }
 uint32_t duration;
 Trex trex;
 if (!mDataSource->getUInt32(data_offset + 4, &trex.track_ID) ||
 !mDataSource->getUInt32(data_offset + 8, &trex.default_sample_description_index) ||
 !mDataSource->getUInt32(data_offset + 12, &trex.default_sample_duration) ||
 !mDataSource->getUInt32(data_offset + 16, &trex.default_sample_size) ||
 !mDataSource->getUInt32(data_offset + 20, &trex.default_sample_flags)) {
 return ERROR_IO;
 }
            mTrex.add(trex);
 break;
 }

 case FOURCC('t', 'x', '3', 'g'):
 {
 uint32_t type;
 const void *data;
 size_t size = 0;
 if (!mLastTrack->meta->findData(
                    kKeyTextFormatData, &type, &data, &size)) {

                 size = 0;
             }
 
            if ((chunk_size > SIZE_MAX) || (SIZE_MAX - chunk_size <= size)) {
                 return ERROR_MALFORMED;
             }
 
 uint8_t *buffer = new uint8_t[size + chunk_size];
 if (buffer == NULL) {
 return ERROR_MALFORMED;
 }

 if (size > 0) {
                memcpy(buffer, data, size);
 }

 if ((size_t)(mDataSource->readAt(*offset, buffer + size, chunk_size))
 < chunk_size) {
 delete[] buffer;
                buffer = NULL;

 *offset += chunk_size;
 return ERROR_IO;
 }

            mLastTrack->meta->setData(
                    kKeyTextFormatData, 0, buffer, size + chunk_size);

 delete[] buffer;

 *offset += chunk_size;
 break;
 }

 case FOURCC('c', 'o', 'v', 'r'):
 {
 *offset += chunk_size;

 if (mFileMetaData != NULL) {
                ALOGV("chunk_data_size = %lld and data_offset = %lld",
                        chunk_data_size, data_offset);

 if (chunk_data_size >= SIZE_MAX - 1) {
 return ERROR_MALFORMED;
 }
                sp<ABuffer> buffer = new ABuffer(chunk_data_size + 1);
 if (mDataSource->readAt(
                    data_offset, buffer->data(), chunk_data_size) != (ssize_t)chunk_data_size) {
 return ERROR_IO;
 }
 const int kSkipBytesOfDataBox = 16;
 if (chunk_data_size <= kSkipBytesOfDataBox) {
 return ERROR_MALFORMED;
 }

                mFileMetaData->setData(
                    kKeyAlbumArt, MetaData::TYPE_NONE,
                    buffer->data() + kSkipBytesOfDataBox, chunk_data_size - kSkipBytesOfDataBox);
 }

 break;
 }

 case FOURCC('t', 'i', 't', 'l'):
 case FOURCC('p', 'e', 'r', 'f'):
 case FOURCC('a', 'u', 't', 'h'):
 case FOURCC('g', 'n', 'r', 'e'):
 case FOURCC('a', 'l', 'b', 'm'):
 case FOURCC('y', 'r', 'r', 'c'):
 {
 *offset += chunk_size;

 status_t err = parse3GPPMetaData(data_offset, chunk_data_size, depth);

 if (err != OK) {
 return err;
 }

 break;
 }

 case FOURCC('I', 'D', '3', '2'):
 {
 *offset += chunk_size;

 if (chunk_data_size < 6) {
 return ERROR_MALFORMED;
 }

            parseID3v2MetaData(data_offset + 6);

 break;
 }

 case FOURCC('-', '-', '-', '-'):
 {
            mLastCommentMean.clear();
            mLastCommentName.clear();
            mLastCommentData.clear();
 *offset += chunk_size;
 break;
 }

 case FOURCC('s', 'i', 'd', 'x'):
 {
            parseSegmentIndex(data_offset, chunk_data_size);
 *offset += chunk_size;
 return UNKNOWN_ERROR; // stop parsing after sidx
 }

 default:
 {
 *offset += chunk_size;
 break;
 }
 }

 return OK;
}
