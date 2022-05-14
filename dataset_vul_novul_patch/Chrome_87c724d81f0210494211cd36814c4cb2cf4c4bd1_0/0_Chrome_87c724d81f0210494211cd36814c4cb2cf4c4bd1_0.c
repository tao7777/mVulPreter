 void ClipboardMessageFilter::OnReadImageReply(
    const SkBitmap& bitmap, IPC::Message* reply_msg) {
   base::SharedMemoryHandle image_handle = base::SharedMemory::NULLHandle();
   uint32 image_size = 0;
   std::string reply_data;
  if (!bitmap.isNull()) {
    std::vector<unsigned char> png_data;
    SkAutoLockPixels lock(bitmap);
    if (gfx::PNGCodec::EncodeWithCompressionLevel(
            static_cast<const unsigned char*>(bitmap.getPixels()),
            gfx::PNGCodec::FORMAT_BGRA,
            gfx::Size(bitmap.width(), bitmap.height()),
            bitmap.rowBytes(),
            false,
            std::vector<gfx::PNGCodec::Comment>(),
            Z_BEST_SPEED,
            &png_data)) {
      base::SharedMemory buffer;
      if (buffer.CreateAndMapAnonymous(png_data.size())) {
        memcpy(buffer.memory(), vector_as_array(&png_data), png_data.size());
        if (buffer.GiveToProcess(peer_handle(), &image_handle)) {
          image_size = png_data.size();
        }
      }
    }
  }
  ClipboardHostMsg_ReadImage::WriteReplyParams(reply_msg, image_handle,
                                               image_size);
  Send(reply_msg);
}
