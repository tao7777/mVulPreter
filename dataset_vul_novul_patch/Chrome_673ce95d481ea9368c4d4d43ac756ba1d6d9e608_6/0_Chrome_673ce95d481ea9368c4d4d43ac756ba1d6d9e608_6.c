  base::SharedMemoryHandle CreateMSKPInSharedMemory() {
    SkDynamicMemoryWStream stream;
    sk_sp<SkDocument> doc = SkMakeMultiPictureDocument(&stream);
    cc::SkiaPaintCanvas canvas(doc->beginPage(800, 600));
    SkRect rect = SkRect::MakeXYWH(10, 10, 250, 250);
    cc::PaintFlags flags;
    flags.setAntiAlias(false);
    flags.setColor(SK_ColorRED);
    flags.setStyle(cc::PaintFlags::kFill_Style);
    canvas.drawRect(rect, flags);
    doc->endPage();
    doc->close();

    size_t len = stream.bytesWritten();
    base::SharedMemoryCreateOptions options;
    options.size = len;
    options.share_read_only = true;

     base::SharedMemory shared_memory;
     if (shared_memory.Create(options) && shared_memory.Map(len)) {
       stream.copyTo(shared_memory.memory());
      return shared_memory.GetReadOnlyHandle();
     }
     return base::SharedMemoryHandle();
   }
