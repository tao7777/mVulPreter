sp<MediaSource> OMXCodec::Create(
 const sp<IOMX> &omx,
 const sp<MetaData> &meta, bool createEncoder,
 const sp<MediaSource> &source,
 const char *matchComponentName,
 uint32_t flags,
 const sp<ANativeWindow> &nativeWindow) {
 int32_t requiresSecureBuffers;
 if (source->getFormat()->findInt32(
                kKeyRequiresSecureBuffers,
 &requiresSecureBuffers)
 && requiresSecureBuffers) {
        flags |= kIgnoreCodecSpecificData;
        flags |= kUseSecureInputBuffers;
 }

 const char *mime;
 bool success = meta->findCString(kKeyMIMEType, &mime);
    CHECK(success);

 Vector<CodecNameAndQuirks> matchingCodecs;
    findMatchingCodecs(
            mime, createEncoder, matchComponentName, flags, &matchingCodecs);

 if (matchingCodecs.isEmpty()) {
        ALOGV("No matching codecs! (mime: %s, createEncoder: %s, "
 "matchComponentName: %s, flags: 0x%x)",
                mime, createEncoder ? "true" : "false", matchComponentName, flags);
 return NULL;
 }

    sp<OMXCodecObserver> observer = new OMXCodecObserver;
    IOMX::node_id node = 0;

 for (size_t i = 0; i < matchingCodecs.size(); ++i) {
 const char *componentNameBase = matchingCodecs[i].mName.string();
 uint32_t quirks = matchingCodecs[i].mQuirks;
 const char *componentName = componentNameBase;

 AString tmp;
 if (flags & kUseSecureInputBuffers) {
            tmp = componentNameBase;
            tmp.append(".secure");

            componentName = tmp.c_str();
 }

 if (createEncoder) {
            sp<MediaSource> softwareCodec =
 InstantiateSoftwareEncoder(componentName, source, meta);

 if (softwareCodec != NULL) {
                ALOGV("Successfully allocated software codec '%s'", componentName);

 return softwareCodec;
 }
 }

 
         ALOGV("Attempting to allocate OMX node '%s'", componentName);
 
        if (!createEncoder
                && (quirks & kOutputBuffersAreUnreadable)
                && (flags & kClientNeedsFramebuffer)) {
            if (strncmp(componentName, "OMX.SEC.", 8)) {
                ALOGW("Component '%s' does not give the client access to "
                     "the framebuffer contents. Skipping.",
                     componentName);
                continue;
            }
        }
         status_t err = omx->allocateNode(componentName, observer, &node);
         if (err == OK) {
             ALOGV("Successfully allocated OMX node '%s'", componentName);

            sp<OMXCodec> codec = new OMXCodec(
                    omx, node, quirks, flags,
                    createEncoder, mime, componentName,
                    source, nativeWindow);

            observer->setCodec(codec);

            err = codec->configureCodec(meta);
 if (err == OK) {
 return codec;
 }

            ALOGV("Failed to configure codec '%s'", componentName);
 }
 }

 return NULL;
}
