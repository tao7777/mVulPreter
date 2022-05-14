status_t BnGraphicBufferProducer::onTransact(
 uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
 switch(code) {
 case REQUEST_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int bufferIdx   = data.readInt32();
            sp<GraphicBuffer> buffer;
 int result = requestBuffer(bufferIdx, &buffer);
            reply->writeInt32(buffer != 0);
 if (buffer != 0) {
                reply->write(*buffer);
 }
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_BUFFER_COUNT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int bufferCount = data.readInt32();
 int result = setBufferCount(bufferCount);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case DEQUEUE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 bool async = static_cast<bool>(data.readInt32());
 uint32_t width = data.readUint32();
 uint32_t height = data.readUint32();
 PixelFormat format = static_cast<PixelFormat>(data.readInt32());
 uint32_t usage = data.readUint32();
 int buf = 0;
            sp<Fence> fence;
 int result = dequeueBuffer(&buf, &fence, async, width, height,
                    format, usage);
            reply->writeInt32(buf);
            reply->writeInt32(fence != NULL);
 if (fence != NULL) {
                reply->write(*fence);
 }
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case DETACH_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int slot = data.readInt32();
 int result = detachBuffer(slot);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case DETACH_NEXT_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            sp<GraphicBuffer> buffer;
            sp<Fence> fence;
 int32_t result = detachNextBuffer(&buffer, &fence);
            reply->writeInt32(result);
 if (result == NO_ERROR) {
                reply->writeInt32(buffer != NULL);
 if (buffer != NULL) {
                    reply->write(*buffer);
 }
                reply->writeInt32(fence != NULL);
 if (fence != NULL) {
                    reply->write(*fence);
 }
 }
 return NO_ERROR;
 }
 case ATTACH_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            sp<GraphicBuffer> buffer = new GraphicBuffer();
            data.read(*buffer.get());
 int slot = 0;
 int result = attachBuffer(&slot, buffer);
            reply->writeInt32(slot);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case QUEUE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int buf = data.readInt32();
 QueueBufferInput input(data);
 QueueBufferOutput* const output =
 reinterpret_cast<QueueBufferOutput *>(
                            reply->writeInplace(sizeof(QueueBufferOutput)));
            memset(output, 0, sizeof(QueueBufferOutput));
 status_t result = queueBuffer(buf, input, output);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case CANCEL_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int buf = data.readInt32();
            sp<Fence> fence = new Fence();
            data.read(*fence.get());
            cancelBuffer(buf, fence);
 return NO_ERROR;
 }
 case QUERY: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int value = 0;
 int what = data.readInt32();
 int res = query(what, &value);
            reply->writeInt32(value);
            reply->writeInt32(res);
 return NO_ERROR;
 }
 case CONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            sp<IProducerListener> listener;
 if (data.readInt32() == 1) {
                listener = IProducerListener::asInterface(data.readStrongBinder());
 }
 int api = data.readInt32();
 bool producerControlledByApp = data.readInt32();

             QueueBufferOutput* const output =
                     reinterpret_cast<QueueBufferOutput *>(
                             reply->writeInplace(sizeof(QueueBufferOutput)));
            memset(output, 0, sizeof(QueueBufferOutput));
             status_t res = connect(listener, api, producerControlledByApp, output);
             reply->writeInt32(res);
             return NO_ERROR;
 }
 case DISCONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int api = data.readInt32();
 status_t res = disconnect(api);
            reply->writeInt32(res);
 return NO_ERROR;
 }
 case SET_SIDEBAND_STREAM: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            sp<NativeHandle> stream;
 if (data.readInt32()) {
                stream = NativeHandle::create(data.readNativeHandle(), true);
 }
 status_t result = setSidebandStream(stream);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case ALLOCATE_BUFFERS: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 bool async = static_cast<bool>(data.readInt32());
 uint32_t width = data.readUint32();
 uint32_t height = data.readUint32();
 PixelFormat format = static_cast<PixelFormat>(data.readInt32());
 uint32_t usage = data.readUint32();
            allocateBuffers(async, width, height, format, usage);
 return NO_ERROR;
 }
 case ALLOW_ALLOCATION: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 bool allow = static_cast<bool>(data.readInt32());
 status_t result = allowAllocation(allow);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case SET_GENERATION_NUMBER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 uint32_t generationNumber = data.readUint32();
 status_t result = setGenerationNumber(generationNumber);
            reply->writeInt32(result);
 return NO_ERROR;
 }
 case GET_CONSUMER_NAME: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            reply->writeString8(getConsumerName());
 return NO_ERROR;
 }
 }
 return BBinder::onTransact(code, data, reply, flags);
}
