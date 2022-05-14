static SharedMemorySupport DoQuerySharedMemorySupport(Display* dpy) {
  int dummy;
  Bool pixmaps_supported;
  if (!XShmQueryVersion(dpy, &dummy, &dummy, &pixmaps_supported))
    return SHARED_MEMORY_NONE;

#if defined(OS_FREEBSD)
  int allow_removed;
  size_t length = sizeof(allow_removed);

  if ((sysctlbyname("kern.ipc.shm_allow_removed", &allow_removed, &length,
      NULL, 0) < 0) || allow_removed < 1) {
    return SHARED_MEMORY_NONE;
  }
 #endif
 
  int shmkey = shmget(IPC_PRIVATE, 1, 0600);
  if (shmkey == -1) {
    LOG(WARNING) << "Failed to get shared memory segment.";
     return SHARED_MEMORY_NONE;
  } else {
    VLOG(1) << "Got shared memory segment " << shmkey;
  }

   void* address = shmat(shmkey, NULL, 0);
   shmctl(shmkey, IPC_RMID, NULL);

  XShmSegmentInfo shminfo;
  memset(&shminfo, 0, sizeof(shminfo));
  shminfo.shmid = shmkey;
 
   gdk_error_trap_push();
   bool result = XShmAttach(dpy, &shminfo);
  if (result)
    VLOG(1) << "X got shared memory segment " << shmkey;
  else
    LOG(WARNING) << "X failed to attach to shared memory segment " << shmkey;
   XSync(dpy, False);
   if (gdk_error_trap_pop())
     result = false;
   shmdt(address);
  if (!result) {
    LOG(WARNING) << "X failed to attach to shared memory segment " << shmkey;
     return SHARED_MEMORY_NONE;
  }

  VLOG(1) << "X attached to shared memory segment " << shmkey;
 
   XShmDetach(dpy, &shminfo);
   return pixmaps_supported ? SHARED_MEMORY_PIXMAP : SHARED_MEMORY_PUTIMAGE;
}
