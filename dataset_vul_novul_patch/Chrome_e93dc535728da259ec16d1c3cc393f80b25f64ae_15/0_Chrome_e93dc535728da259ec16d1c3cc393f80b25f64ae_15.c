static STGMEDIUM* GetStorageForFileDescriptor(
    const base::FilePath& path) {
   base::string16 file_name = path.value();
   DCHECK(!file_name.empty());
   HANDLE hdata = GlobalAlloc(GPTR, sizeof(FILEGROUPDESCRIPTOR));
  base::win::ScopedHGlobal<FILEGROUPDESCRIPTOR*> locked_mem(hdata);
 
   FILEGROUPDESCRIPTOR* descriptor = locked_mem.get();
   descriptor->cItems = 1;
  descriptor->fgd[0].dwFlags = FD_LINKUI;
  wcsncpy_s(descriptor->fgd[0].cFileName, MAX_PATH, file_name.c_str(),
            std::min(file_name.size(), static_cast<size_t>(MAX_PATH - 1u)));

  STGMEDIUM* storage = new STGMEDIUM;
  storage->tymed = TYMED_HGLOBAL;
  storage->hGlobal = hdata;
  storage->pUnkForRelease = NULL;
  return storage;
}
