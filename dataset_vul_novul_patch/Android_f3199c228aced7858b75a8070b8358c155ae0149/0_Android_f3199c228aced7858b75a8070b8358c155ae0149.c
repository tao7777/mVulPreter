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
                    size_t heapSize = mHeap->getSize();
                    if (s <= heapSize
                            && o >= 0
                            && (static_cast<size_t>(o) <= heapSize - s)) {
                        mOffset = o;
                        mSize = s;
                    } else {
                        // Hm.
                        android_errorWriteWithInfoLog(0x534e4554,
                            "26877992", -1, NULL, 0);
                        mOffset = 0;
                        mSize = 0;
                    }
                 }
             }
         }
     }
     if (offset) *offset = mOffset;
     if (size) *size = mSize;
    return (mSize > 0) ? mHeap : 0;
 }
