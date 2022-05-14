void ClipboardMessageFilter::OnWriteObjectsSync(
    const ui::Clipboard::ObjectMap& objects,
    base::SharedMemoryHandle bitmap_handle) {
  DCHECK(base::SharedMemory::IsHandleValid(bitmap_handle))
      << "Bad bitmap handle";

   scoped_ptr<ui::Clipboard::ObjectMap> long_living_objects(
       new ui::Clipboard::ObjectMap(objects));
  SanitizeObjectMap(long_living_objects.get(), kAllowBitmap);
  if (!ui::Clipboard::ReplaceSharedMemHandle(
           long_living_objects.get(), bitmap_handle, PeerHandle()))
    return;

  BrowserThread::PostTask(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(&WriteObjectsOnUIThread,
                 base::Owned(long_living_objects.release())));
}
