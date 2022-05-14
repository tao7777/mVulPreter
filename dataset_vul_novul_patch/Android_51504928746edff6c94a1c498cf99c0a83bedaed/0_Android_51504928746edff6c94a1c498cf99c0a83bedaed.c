 virtual ssize_t readAt(off64_t offset, void *buffer, size_t size) {
 Parcel data, reply;
        data.writeInterfaceToken(
 IMediaHTTPConnection::getInterfaceDescriptor());

        data.writeInt64(offset);
        data.writeInt32(size);

 status_t err = remote()->transact(READ_AT, data, &reply);
 if (err != OK) {
            ALOGE("remote readAt failed");
 return UNKNOWN_ERROR;
 }

 int32_t exceptionCode = reply.readExceptionCode();

 if (exceptionCode) {

             return UNKNOWN_ERROR;
         }
 
        size_t len = reply.readInt32();
 
        if (len > size) {
            ALOGE("requested %zu, got %zu", size, len);
            return ERROR_OUT_OF_RANGE;
         }
        if (len > mMemory->size()) {
            ALOGE("got %zu, but memory has %zu", len, mMemory->size());
            return ERROR_OUT_OF_RANGE;
        }

        memcpy(buffer, mMemory->pointer(), len);
 
         return len;
     }
