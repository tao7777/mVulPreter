V4L2JpegEncodeAccelerator::JobRecord::JobRecord(
    scoped_refptr<VideoFrame> input_frame,
    scoped_refptr<VideoFrame> output_frame,
    int quality,
    int32_t task_id,
    BitstreamBuffer* exif_buffer)
    : input_frame(input_frame),
       output_frame(output_frame),
       quality(quality),
       task_id(task_id),
      output_shm(base::subtle::PlatformSharedMemoryRegion(), 0, true),  // dummy
       exif_shm(nullptr) {
   if (exif_buffer) {
     exif_shm.reset(new UnalignedSharedMemory(exif_buffer->TakeRegion(),
                                             exif_buffer->size(), false));
    exif_offset = exif_buffer->offset();
  }
}
