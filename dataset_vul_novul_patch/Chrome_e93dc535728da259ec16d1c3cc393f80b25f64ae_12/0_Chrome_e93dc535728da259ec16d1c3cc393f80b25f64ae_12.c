static STGMEDIUM* GetIDListStorageForFileName(const base::FilePath& path) {
  LPITEMIDLIST pidl = GetPidlFromPath(path);
  if (!pidl)
    return NULL;

  const size_t kPIDLCountSize = sizeof(UINT);
  const size_t kPIDLOffsetsSize = 2 * sizeof(UINT);
  const size_t kFirstPIDLOffset = kPIDLCountSize + kPIDLOffsetsSize;
  const size_t kFirstPIDLSize = 2;  // Empty PIDL - 2 NULL bytes.
  const size_t kSecondPIDLSize = PIDLSize(pidl);
   const size_t kCIDASize = kFirstPIDLOffset + kFirstPIDLSize + kSecondPIDLSize;
   HANDLE hdata = GlobalAlloc(GMEM_MOVEABLE, kCIDASize);
 
  base::win::ScopedHGlobal<CIDA*> locked_mem(hdata);
   CIDA* cida = locked_mem.get();
   cida->cidl = 1;     // We have one PIDL (not including the 0th root PIDL).
   cida->aoffset[0] = kFirstPIDLOffset;
  cida->aoffset[1] = kFirstPIDLOffset + kFirstPIDLSize;
  LPITEMIDLIST idl = GetNthPIDL(cida, 0);
  idl->mkid.cb = 0;
  idl->mkid.abID[0] = 0;
  idl = GetNthPIDL(cida, 1);
  memcpy(idl, pidl, kSecondPIDLSize);

  STGMEDIUM* storage = new STGMEDIUM;
  storage->tymed = TYMED_HGLOBAL;
  storage->hGlobal = hdata;
  storage->pUnkForRelease = NULL;
  return storage;
}
