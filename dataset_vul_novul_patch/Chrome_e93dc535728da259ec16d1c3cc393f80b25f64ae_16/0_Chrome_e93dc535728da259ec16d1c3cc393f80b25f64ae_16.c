static STGMEDIUM* GetStorageForFileName(const base::FilePath& path) {
  const size_t kDropSize = sizeof(DROPFILES);
  const size_t kTotalBytes =
       kDropSize + (path.value().length() + 2) * sizeof(wchar_t);
   HANDLE hdata = GlobalAlloc(GMEM_MOVEABLE, kTotalBytes);
 
  base::win::ScopedHGlobal<DROPFILES*> locked_mem(hdata);
   DROPFILES* drop_files = locked_mem.get();
   drop_files->pFiles = sizeof(DROPFILES);
   drop_files->fWide = TRUE;
  wchar_t* data = reinterpret_cast<wchar_t*>(
      reinterpret_cast<BYTE*>(drop_files) + kDropSize);
  const size_t copy_size = (path.value().length() + 1) * sizeof(wchar_t);
  memcpy(data, path.value().c_str(), copy_size);
  data[path.value().length() + 1] = L'\0';  // Double NULL

  STGMEDIUM* storage = new STGMEDIUM;
  storage->tymed = TYMED_HGLOBAL;
  storage->hGlobal = hdata;
  storage->pUnkForRelease = NULL;
  return storage;
}
