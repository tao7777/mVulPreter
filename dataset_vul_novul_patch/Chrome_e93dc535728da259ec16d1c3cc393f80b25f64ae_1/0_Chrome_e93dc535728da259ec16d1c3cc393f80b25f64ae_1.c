bool ClipboardUtil::GetFileContents(IDataObject* data_object,
    base::string16* filename, std::string* file_contents) {
  DCHECK(data_object && filename && file_contents);
  if (!HasData(data_object, Clipboard::GetFileContentZeroFormatType()) &&
      !HasData(data_object, Clipboard::GetFileDescriptorFormatType()))
    return false;

  STGMEDIUM content;
   if (GetData(
           data_object, Clipboard::GetFileContentZeroFormatType(), &content)) {
     if (TYMED_HGLOBAL == content.tymed) {
      base::win::ScopedHGlobal<char*> data(content.hGlobal);
       file_contents->assign(data.get(), data.Size());
     }
     ReleaseStgMedium(&content);
  }

  STGMEDIUM description;
  if (GetData(data_object,
               Clipboard::GetFileDescriptorFormatType(),
               &description)) {
     {
      base::win::ScopedHGlobal<FILEGROUPDESCRIPTOR*> fgd(description.hGlobal);
       DCHECK_GE(fgd->cItems, 1u);
       filename->assign(fgd->fgd[0].cFileName);
    }
    ReleaseStgMedium(&description);
  }
  return true;
}
