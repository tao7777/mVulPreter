bool DeleteSymlink(const FilePath& file_path) {
  const bool deleted = HANDLE_EINTR(unlink(file_path.value().c_str())) == 0;
  return deleted;
}
