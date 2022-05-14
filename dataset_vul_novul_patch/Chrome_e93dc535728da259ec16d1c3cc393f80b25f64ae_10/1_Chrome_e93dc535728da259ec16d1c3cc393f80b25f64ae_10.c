 bool ClipboardUtil::HasUrl(IDataObject* data_object, bool convert_filenames) {
   DCHECK(data_object);
   return HasData(data_object, Clipboard::GetMozUrlFormatType()) ||
          HasData(data_object, Clipboard::GetUrlWFormatType()) ||
          HasData(data_object, Clipboard::GetUrlFormatType()) ||
         (convert_filenames && (
             HasData(data_object, Clipboard::GetFilenameWFormatType()) ||
             HasData(data_object, Clipboard::GetFilenameFormatType())));
 }
