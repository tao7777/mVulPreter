void ClipboardMessageFilter::OnWriteObjectsAsync(
    const ui::Clipboard::ObjectMap& objects) {
   scoped_ptr<ui::Clipboard::ObjectMap> sanitized_objects(
       new ui::Clipboard::ObjectMap(objects));
  SanitizeObjectMap(sanitized_objects.get(), kFilterBitmap);
 
 #if defined(OS_WIN)
  BrowserThread::PostTask(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(
          &WriteObjectsOnUIThread, base::Owned(sanitized_objects.release())));
#else
  GetClipboard()->WriteObjects(
      ui::CLIPBOARD_TYPE_COPY_PASTE, *sanitized_objects.get());
#endif
}
