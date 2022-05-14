 static STGMEDIUM* GetStorageForBytes(const void* data, size_t bytes) {
   HANDLE handle = GlobalAlloc(GPTR, static_cast<int>(bytes));
   if (handle) {
    base::win::ScopedHGlobal<uint8*> scoped(handle);
     memcpy(scoped.get(), data, bytes);
   }
 
  STGMEDIUM* storage = new STGMEDIUM;
  storage->hGlobal = handle;
  storage->tymed = TYMED_HGLOBAL;
  storage->pUnkForRelease = NULL;
  return storage;
}
