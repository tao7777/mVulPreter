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
 
        int32_t len = reply.readInt32();
 
        if (len > 0) {
            memcpy(buffer, mMemory->pointer(), len);
         }
 
         return len;
     }
