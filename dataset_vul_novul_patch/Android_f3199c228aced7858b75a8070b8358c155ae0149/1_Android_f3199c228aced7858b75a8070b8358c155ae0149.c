sp<IMemoryHeap> BpMemory::getMemory(ssize_t* offset, size_t* size) const
{
 if (mHeap == 0) {
 Parcel data, reply;
        data.writeInterfaceToken(IMemory::getInterfaceDescriptor());
 if (remote()->transact(GET_MEMORY, data, &reply) == NO_ERROR) {
            sp<IBinder> heap = reply.readStrongBinder();
 ssize_t o = reply.readInt32();
 size_t s = reply.readInt32();

             if (heap != 0) {
                 mHeap = interface_cast<IMemoryHeap>(heap);
                 if (mHeap != 0) {
                    mOffset = o;
                    mSize = s;
                 }
             }
         }
     }
     if (offset) *offset = mOffset;
     if (size) *size = mSize;
    return mHeap;
 }
