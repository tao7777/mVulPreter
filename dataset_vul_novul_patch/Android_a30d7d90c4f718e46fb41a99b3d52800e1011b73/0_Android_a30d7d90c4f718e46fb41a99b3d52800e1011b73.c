status_t BnGraphicBufferConsumer::onTransact(
 uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
 switch(code) {
 case ACQUIRE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 BufferItem item;
 int64_t presentWhen = data.readInt64();
 uint64_t maxFrameNumber = data.readUint64();
 status_t result = acquireBuffer(&item, presentWhen, maxFrameNumber);
 status_t err = reply->write(item);
 if (err) return err;
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case DETACH_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 int slot = data.readInt32();
 int result = detachBuffer(slot);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case ATTACH_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            sp<GraphicBuffer> buffer = new GraphicBuffer();
            data.read(*buffer.get());
 int slot = -1;
 int result = attachBuffer(&slot, buffer);
            reply->writeInt32(slot);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case RELEASE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 int buf = data.readInt32();
 uint64_t frameNumber = static_cast<uint64_t>(data.readInt64());
            sp<Fence> releaseFence = new Fence();
 status_t err = data.read(*releaseFence);
 if (err) return err;
 status_t result = releaseBuffer(buf, frameNumber,
                    EGL_NO_DISPLAY, EGL_NO_SYNC_KHR, releaseFence);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case CONSUMER_CONNECT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            sp<IConsumerListener> consumer = IConsumerListener::asInterface( data.readStrongBinder() );
 bool controlledByApp = data.readInt32();
 status_t result = consumerConnect(consumer, controlledByApp);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case CONSUMER_DISCONNECT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 status_t result = consumerDisconnect();
            reply->writeInt32(result);
 return NO_ERROR;

         }
         case GET_RELEASED_BUFFERS: {
             CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            uint64_t slotMask = 0;
             status_t result = getReleasedBuffers(&slotMask);
             reply->writeInt64(static_cast<int64_t>(slotMask));
             reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_DEFAULT_BUFFER_SIZE: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 uint32_t width = data.readUint32();
 uint32_t height = data.readUint32();
 status_t result = setDefaultBufferSize(width, height);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_DEFAULT_MAX_BUFFER_COUNT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 int bufferCount = data.readInt32();
 status_t result = setDefaultMaxBufferCount(bufferCount);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case DISABLE_ASYNC_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 status_t result = disableAsyncBuffer();
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_MAX_ACQUIRED_BUFFER_COUNT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 int maxAcquiredBuffers = data.readInt32();
 status_t result = setMaxAcquiredBufferCount(maxAcquiredBuffers);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_CONSUMER_NAME: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            setConsumerName( data.readString8() );
 return NO_ERROR;
 }
 case SET_DEFAULT_BUFFER_FORMAT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 PixelFormat defaultFormat = static_cast<PixelFormat>(data.readInt32());
 status_t result = setDefaultBufferFormat(defaultFormat);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_DEFAULT_BUFFER_DATA_SPACE: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            android_dataspace defaultDataSpace =
 static_cast<android_dataspace>(data.readInt32());
 status_t result = setDefaultBufferDataSpace(defaultDataSpace);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_CONSUMER_USAGE_BITS: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 uint32_t usage = data.readUint32();
 status_t result = setConsumerUsageBits(usage);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_TRANSFORM_HINT: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 uint32_t hint = data.readUint32();
 status_t result = setTransformHint(hint);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case GET_SIDEBAND_STREAM: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
            sp<NativeHandle> stream = getSidebandStream();
            reply->writeInt32(static_cast<int32_t>(stream != NULL));
 if (stream != NULL) {
                reply->writeNativeHandle(stream->handle());
 }
 return NO_ERROR;
 }
 case DUMP: {
            CHECK_INTERFACE(IGraphicBufferConsumer, data, reply);
 String8 result = data.readString8();
 String8 prefix = data.readString8();
 static_cast<IGraphicBufferConsumer*>(this)->dump(result, prefix);
            reply->writeString8(result);
 return NO_ERROR;
 }
 }
 return BBinder::onTransact(code, data, reply, flags);
}
