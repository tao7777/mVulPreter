 bool ClipboardUtil::GetFilenames(IDataObject* data_object,
                                 std::vector<base::string16>* filenames) {
  DCHECK(data_object && filenames);
  if (!HasFilenames(data_object))
     return false;
 
   STGMEDIUM medium;
  if (!GetData(data_object, Clipboard::GetCFHDropFormatType(), &medium))
    return false;
 
  HDROP hdrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
  if (!hdrop)
    return false;
 
  const int kMaxFilenameLen = 4096;
  const unsigned num_files = DragQueryFileW(hdrop, 0xffffffff, 0, 0);
  for (unsigned int i = 0; i < num_files; ++i) {
    wchar_t filename[kMaxFilenameLen];
    if (!DragQueryFileW(hdrop, i, filename, kMaxFilenameLen))
      continue;
    filenames->push_back(filename);
   }
 
  DragFinish(hdrop);
  GlobalUnlock(medium.hGlobal);
  return true;
 }
