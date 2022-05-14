 bool ClipboardUtil::HasFilenames(IDataObject* data_object) {
   DCHECK(data_object);
  return HasData(data_object, Clipboard::GetCFHDropFormatType()) ||
         HasData(data_object, Clipboard::GetFilenameWFormatType()) ||
         HasData(data_object, Clipboard::GetFilenameFormatType());
 }
