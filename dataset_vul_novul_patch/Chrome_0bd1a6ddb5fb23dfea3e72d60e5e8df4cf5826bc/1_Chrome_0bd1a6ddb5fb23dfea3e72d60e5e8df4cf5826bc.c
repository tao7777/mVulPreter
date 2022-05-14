bool BackingStoreGtk::CopyFromBackingStore(const gfx::Rect& rect,
                                           skia::PlatformCanvas* output) {
  base::TimeTicks begin_time = base::TimeTicks::Now();

  if (visual_depth_ < 24) {
    return false;
  }

  const int width = std::min(size().width(), rect.width());
  const int height = std::min(size().height(), rect.height());

  XImage* image;
  XShmSegmentInfo shminfo;  // Used only when shared memory is enabled.
  if (shared_memory_support_ != ui::SHARED_MEMORY_NONE) {
    Visual* visual = static_cast<Visual*>(visual_);
    memset(&shminfo, 0, sizeof(shminfo));
    image = XShmCreateImage(display_, visual, 32,
                            ZPixmap, NULL, &shminfo, width, height);
    if (!image) {
      return false;
    }
    if (image->bytes_per_line == 0 || image->height == 0 ||
        static_cast<size_t>(image->height) >
        (std::numeric_limits<size_t>::max() / image->bytes_per_line)) {
      XDestroyImage(image);
       return false;
     }
     shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height,
                           IPC_CREAT|0666);
     if (shminfo.shmid == -1) {
       XDestroyImage(image);
       return false;
     }
 
     void* mapped_memory = shmat(shminfo.shmid, NULL, SHM_RDONLY);
    shmctl(shminfo.shmid, IPC_RMID, 0);
    if (mapped_memory == (void*)-1) {
      XDestroyImage(image);
      return false;
    }
    shminfo.shmaddr = image->data = static_cast<char*>(mapped_memory);

    if (!XShmAttach(display_, &shminfo) ||
         !XShmGetImage(display_, pixmap_, image, rect.x(), rect.y(),
                       AllPlanes)) {
       DestroySharedImage(display_, image, &shminfo);
       return false;
     }
   } else {
     image = XGetImage(display_, pixmap_,
                       rect.x(), rect.y(), width, height,
                      AllPlanes, ZPixmap);
  }

  if (!output->initialize(width, height, true) ||
      image->bits_per_pixel != 32) {
    if (shared_memory_support_ != ui::SHARED_MEMORY_NONE)
      DestroySharedImage(display_, image, &shminfo);
    else
      XDestroyImage(image);
    return false;
  }

  SkBitmap bitmap = skia::GetTopDevice(*output)->accessBitmap(true);
  SkAutoLockPixels alp(bitmap);

  for (int y = 0; y < height; y++) {
    const uint32* src_row = reinterpret_cast<uint32*>(
        &image->data[image->bytes_per_line * y]);
    uint32* dest_row = bitmap.getAddr32(0, y);
    for (int x = 0; x < width; ++x, ++dest_row) {
      *dest_row = src_row[x] | 0xff000000;
    }
  }

  if (shared_memory_support_ != ui::SHARED_MEMORY_NONE)
    DestroySharedImage(display_, image, &shminfo);
  else
    XDestroyImage(image);

  HISTOGRAM_TIMES("BackingStore.RetrievalFromX",
                  base::TimeTicks::Now() - begin_time);
  return true;
}
