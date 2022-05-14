XSharedMemoryId AttachSharedMemory(Display* display, int shared_memory_key) {
  DCHECK(QuerySharedMemorySupport(display));

  XShmSegmentInfo shminfo;
  memset(&shminfo, 0, sizeof(shminfo));
  shminfo.shmid = shared_memory_key;

  if (!XShmAttach(display, &shminfo)) {
    LOG(WARNING) << "X failed to attach to shared memory segment "
                 << shminfo.shmid;
     NOTREACHED();
  } else {
    VLOG(1) << "X attached to shared memory segment " << shminfo.shmid;
  }
 
   return shminfo.shmseg;
 }
