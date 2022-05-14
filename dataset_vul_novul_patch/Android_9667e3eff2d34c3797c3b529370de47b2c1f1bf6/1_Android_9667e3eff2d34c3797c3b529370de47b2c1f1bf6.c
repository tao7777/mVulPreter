status_t BnHDCP::onTransact(
 uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags) {
 switch (code) {
 case HDCP_SET_OBSERVER:
 {
            CHECK_INTERFACE(IHDCP, data, reply);

            sp<IHDCPObserver> observer =
                interface_cast<IHDCPObserver>(data.readStrongBinder());

            reply->writeInt32(setObserver(observer));
 return OK;
 }

 case HDCP_INIT_ASYNC:
 {
            CHECK_INTERFACE(IHDCP, data, reply);

 const char *host = data.readCString();
 unsigned port = data.readInt32();

            reply->writeInt32(initAsync(host, port));
 return OK;
 }

 case HDCP_SHUTDOWN_ASYNC:
 {
            CHECK_INTERFACE(IHDCP, data, reply);

            reply->writeInt32(shutdownAsync());
 return OK;
 }

 case HDCP_GET_CAPS:
 {
            CHECK_INTERFACE(IHDCP, data, reply);

            reply->writeInt32(getCaps());
 return OK;
 }


         case HDCP_ENCRYPT:
         {
             size_t size = data.readInt32();
            size_t bufSize = 2 * size;
            if (bufSize > size) {
                inData = malloc(bufSize);
             }
             if (inData == NULL) {
                 reply->writeInt32(ERROR_OUT_OF_RANGE);
                 return OK;
 }

 
             void *outData = (uint8_t *)inData + size;
 
            data.read(inData, size);
 
             uint32_t streamCTR = data.readInt32();
             uint64_t inputCTR;
            status_t err = encrypt(inData, size, streamCTR, &inputCTR, outData);
 
             reply->writeInt32(err);
 
 if (err == OK) {
                reply->writeInt64(inputCTR);
                reply->write(outData, size);
 }

            free(inData);
            inData = outData = NULL;

 return OK;
 }

 case HDCP_ENCRYPT_NATIVE:
 {
            CHECK_INTERFACE(IHDCP, data, reply);

            sp<GraphicBuffer> graphicBuffer = new GraphicBuffer();
            data.read(*graphicBuffer);
 size_t offset = data.readInt32();
 size_t size = data.readInt32();
 uint32_t streamCTR = data.readInt32();
 void *outData = malloc(size);
 uint64_t inputCTR;

 status_t err = encryptNative(graphicBuffer, offset, size,
                                         streamCTR, &inputCTR, outData);

            reply->writeInt32(err);

 if (err == OK) {
                reply->writeInt64(inputCTR);
                reply->write(outData, size);
 }

            free(outData);
            outData = NULL;

 return OK;
 }

 case HDCP_DECRYPT:
 {
 size_t size = data.readInt32();
 size_t bufSize = 2 * size;

 void *inData = NULL;
 if (bufSize > size) {
                inData = malloc(bufSize);
 }

 if (inData == NULL) {
                reply->writeInt32(ERROR_OUT_OF_RANGE);
 return OK;
 }

 void *outData = (uint8_t *)inData + size;

            data.read(inData, size);

 uint32_t streamCTR = data.readInt32();
 uint64_t inputCTR = data.readInt64();
 status_t err = decrypt(inData, size, streamCTR, inputCTR, outData);

            reply->writeInt32(err);

 if (err == OK) {
                reply->write(outData, size);
 }

            free(inData);
            inData = outData = NULL;

 return OK;
 }

 default:
 return BBinder::onTransact(code, data, reply, flags);
 }
}
