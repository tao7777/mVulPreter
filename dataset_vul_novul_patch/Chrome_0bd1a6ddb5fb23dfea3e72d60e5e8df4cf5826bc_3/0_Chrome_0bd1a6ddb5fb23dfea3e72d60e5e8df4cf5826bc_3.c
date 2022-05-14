 TransportDIB* TransportDIB::Create(size_t size, uint32 sequence_num) {
  const int shmkey = shmget(IPC_PRIVATE, size, 0600);
   if (shmkey == -1) {
     DLOG(ERROR) << "Failed to create SysV shared memory region"
                 << " errno:" << errno;
     return NULL;
  } else {
    VLOG(1) << "Created SysV shared memory region " << shmkey;
   }
 
   void* address = shmat(shmkey, NULL /* desired address */, 0 /* flags */);
  shmctl(shmkey, IPC_RMID, 0);
  if (address == kInvalidAddress)
    return NULL;

  TransportDIB* dib = new TransportDIB;

  dib->key_.shmkey = shmkey;
  dib->address_ = address;
  dib->size_ = size;
  return dib;
}
