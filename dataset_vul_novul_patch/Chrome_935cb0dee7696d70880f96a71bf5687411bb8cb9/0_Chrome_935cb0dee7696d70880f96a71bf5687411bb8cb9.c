bool SharedMemory::Create(const SharedMemoryCreateOptions& options) {
  DCHECK(!options.executable);
  DCHECK(!mapped_file_);
  if (options.size == 0)
    return false;

   uint32 rounded_size = (options.size + 0xffff) & ~0xffff;
  if (rounded_size < options.size)
    return false;
   name_ = ASCIIToWide(options.name == NULL ? "" : *options.name);
   mapped_file_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
       PAGE_READWRITE, 0, static_cast<DWORD>(rounded_size),
      name_.empty() ? NULL : name_.c_str());
  if (!mapped_file_)
    return false;

  created_size_ = options.size;

  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    created_size_ = 0;
    if (!options.open_existing) {
      Close();
      return false;
    }
  }

  return true;
}
