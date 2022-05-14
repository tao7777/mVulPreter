 bool ClipboardUtil::GetFilenames(IDataObject* data_object,
                                 std::vector<base::string16>* filenames) {
  DCHECK(data_object && filenames);
  if (!HasFilenames(data_object))
     return false;
 
   STGMEDIUM medium;
  if (GetData(data_object, Clipboard::GetCFHDropFormatType(), &medium)) {
    {
      base::win::ScopedHGlobal<HDROP> hdrop(medium.hGlobal);
      if (!hdrop.get())
        return false;

      const int kMaxFilenameLen = 4096;
      const unsigned num_files = DragQueryFileW(hdrop.get(), 0xffffffff, 0, 0);
      for (unsigned int i = 0; i < num_files; ++i) {
        wchar_t filename[kMaxFilenameLen];
        if (!DragQueryFileW(hdrop.get(), i, filename, kMaxFilenameLen))
          continue;
        filenames->push_back(filename);
      }
    }
    ReleaseStgMedium(&medium);
    return true;
  }
 
  if (GetData(data_object, Clipboard::GetFilenameWFormatType(), &medium)) {
    {
      // filename using unicode
      base::win::ScopedHGlobal<wchar_t*> data(medium.hGlobal);
      if (data.get() && data.get()[0])
        filenames->push_back(data.get());
    }
    ReleaseStgMedium(&medium);
    return true;
  }
 
  if (GetData(data_object, Clipboard::GetFilenameFormatType(), &medium)) {
    {
      // filename using ascii
      base::win::ScopedHGlobal<char*> data(medium.hGlobal);
      if (data.get() && data.get()[0])
        filenames->push_back(base::SysNativeMBToWide(data.get()));
    }
    ReleaseStgMedium(&medium);
    return true;
   }
 
  return false;
 }
