void ResourceMessageFilter::OnClipboardWriteObjectsAsync(
    const Clipboard::ObjectMap& objects) {
   Clipboard::ObjectMap* long_living_objects = new Clipboard::ObjectMap(objects);
 
  // This async message doesn't support shared-memory based bitmaps; they must
  // be removed otherwise we might dereference a rubbish pointer.
  long_living_objects->erase(Clipboard::CBF_SMBITMAP);

   ChromeThread::PostTask(
       ChromeThread::UI,
       FROM_HERE,
      new WriteClipboardTask(long_living_objects));
}
