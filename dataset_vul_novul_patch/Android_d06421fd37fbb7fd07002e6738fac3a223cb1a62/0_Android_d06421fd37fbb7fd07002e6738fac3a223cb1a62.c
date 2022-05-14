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
 } break;
 case SET_BUFFER_COUNT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int bufferCount = data.readInt32();
 int result = setBufferCount(bufferCount);
            reply->writeInt32(result);
 return NO_ERROR;
 } break;
 case DEQUEUE_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 bool async      = data.readInt32();
 uint32_t w      = data.readInt32();
 uint32_t h      = data.readInt32();
 uint32_t format = data.readInt32();
 uint32_t usage  = data.readInt32();
 int buf = 0;
            sp<Fence> fence;
 int result = dequeueBuffer(&buf, &fence, async, w, h, format, usage);
            reply->writeInt32(buf);
            reply->writeInt32(fence != NULL);
 if (fence != NULL) {
                reply->write(*fence);
 }
            reply->writeInt32(result);
 return NO_ERROR;
 } break;
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
 } break;
 case CANCEL_BUFFER: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int buf = data.readInt32();
            sp<Fence> fence = new Fence();
            data.read(*fence.get());
            cancelBuffer(buf, fence);
 return NO_ERROR;
 } break;
 case QUERY: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int value = 0;
 int what = data.readInt32();
 int res = query(what, &value);
            reply->writeInt32(value);
            reply->writeInt32(res);
 return NO_ERROR;
 } break;
 case CONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
            sp<IBinder> token = data.readStrongBinder();
 int api = data.readInt32();
 bool producerControlledByApp = data.readInt32();
 QueueBufferOutput* const output =
 reinterpret_cast<QueueBufferOutput *>(
                            reply->writeInplace(sizeof(QueueBufferOutput)));
 status_t res = connect(token, api, producerControlledByApp, output);
            reply->writeInt32(res);
 return NO_ERROR;
 } break;
 case DISCONNECT: {
            CHECK_INTERFACE(IGraphicBufferProducer, data, reply);
 int api = data.readInt32();
 status_t res = disconnect(api);
            reply->writeInt32(res);
 return NO_ERROR;
 } break;
 }
 return BBinder::onTransact(code, data, reply, flags);
}
