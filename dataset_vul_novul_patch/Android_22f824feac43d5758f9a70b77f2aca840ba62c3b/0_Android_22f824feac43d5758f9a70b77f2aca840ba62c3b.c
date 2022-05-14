status_t BnCrypto::onTransact(
 uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags) {
 switch (code) {
 case INIT_CHECK:
 {
            CHECK_INTERFACE(ICrypto, data, reply);
            reply->writeInt32(initCheck());

 return OK;
 }

 case IS_CRYPTO_SUPPORTED:
 {
            CHECK_INTERFACE(ICrypto, data, reply);
 uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));
            reply->writeInt32(isCryptoSchemeSupported(uuid));

 return OK;
 }

 case CREATE_PLUGIN:
 {
            CHECK_INTERFACE(ICrypto, data, reply);

 uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));

 size_t opaqueSize = data.readInt32();
 void *opaqueData = NULL;

 if (opaqueSize > 0) {
                opaqueData = malloc(opaqueSize);
                data.read(opaqueData, opaqueSize);
 }

            reply->writeInt32(createPlugin(uuid, opaqueData, opaqueSize));

 if (opaqueData != NULL) {
                free(opaqueData);
                opaqueData = NULL;
 }

 return OK;
 }

 case DESTROY_PLUGIN:
 {
            CHECK_INTERFACE(ICrypto, data, reply);
            reply->writeInt32(destroyPlugin());

 return OK;
 }

 case REQUIRES_SECURE_COMPONENT:
 {
            CHECK_INTERFACE(ICrypto, data, reply);

 const char *mime = data.readCString();
            reply->writeInt32(requiresSecureDecoderComponent(mime));

 return OK;
 }

 case DECRYPT:
 {
            CHECK_INTERFACE(ICrypto, data, reply);

 bool secure = data.readInt32() != 0;
 CryptoPlugin::Mode mode = (CryptoPlugin::Mode)data.readInt32();

 uint8_t key[16];
            data.read(key, sizeof(key));

 uint8_t iv[16];
            data.read(iv, sizeof(iv));

 size_t totalSize = data.readInt32();
            sp<IMemory> sharedBuffer =
                interface_cast<IMemory>(data.readStrongBinder());
 int32_t offset = data.readInt32();

 int32_t numSubSamples = data.readInt32();

 CryptoPlugin::SubSample *subSamples =
 new CryptoPlugin::SubSample[numSubSamples];

            data.read(
                    subSamples,
 sizeof(CryptoPlugin::SubSample) * numSubSamples);

 void *secureBufferId, *dstPtr;
 if (secure) {
                secureBufferId = reinterpret_cast<void *>(static_cast<uintptr_t>(data.readInt64()));
 } else {
                dstPtr = calloc(1, totalSize);
 }

 AString errorDetailMsg;
 ssize_t result;

 size_t sumSubsampleSizes = 0;
 bool overflow = false;
 for (int32_t i = 0; i < numSubSamples; ++i) {
 CryptoPlugin::SubSample &ss = subSamples[i];
 if (sumSubsampleSizes <= SIZE_MAX - ss.mNumBytesOfEncryptedData) {
                    sumSubsampleSizes += ss.mNumBytesOfEncryptedData;
 } else {
                    overflow = true;
 }
 if (sumSubsampleSizes <= SIZE_MAX - ss.mNumBytesOfClearData) {
                    sumSubsampleSizes += ss.mNumBytesOfClearData;
 } else {
                    overflow = true;
 }
 }

 
             if (overflow || sumSubsampleSizes != totalSize) {
                 result = -EINVAL;
            } else if (totalSize > sharedBuffer->size()) {
                result = -EINVAL;
            } else if ((size_t)offset > sharedBuffer->size() - totalSize) {
                 result = -EINVAL;
             } else {
                 result = decrypt(
                    secure,
                    key,
                    iv,
                    mode,
                    sharedBuffer, offset,
                    subSamples, numSubSamples,
                    secure ? secureBufferId : dstPtr,
 &errorDetailMsg);
 }

            reply->writeInt32(result);

 if (isCryptoError(result)) {
                reply->writeCString(errorDetailMsg.c_str());
 }

 if (!secure) {
 if (result >= 0) {
                    CHECK_LE(result, static_cast<ssize_t>(totalSize));
                    reply->write(dstPtr, result);
 }
                free(dstPtr);
                dstPtr = NULL;
 }

 delete[] subSamples;
            subSamples = NULL;

 return OK;
 }

 case NOTIFY_RESOLUTION:
 {
            CHECK_INTERFACE(ICrypto, data, reply);

 int32_t width = data.readInt32();
 int32_t height = data.readInt32();
            notifyResolution(width, height);

 return OK;
 }

 case SET_MEDIADRM_SESSION:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
            readVector(data, sessionId);
            reply->writeInt32(setMediaDrmSession(sessionId));
 return OK;
 }

 default:
 return BBinder::onTransact(code, data, reply, flags);
 }
}
