 void FileReaderLoader::OnCalculatedSize(uint64_t total_size,
                                         uint64_t expected_content_size) {
   OnStartLoading(expected_content_size);
   if (expected_content_size == 0) {
     received_all_data_ = true;
     return;
  }

  if (IsSyncLoad()) {
    OnDataPipeReadable(MOJO_RESULT_OK);
  } else {
    handle_watcher_.Watch(
        consumer_handle_.get(), MOJO_HANDLE_SIGNAL_READABLE,
        WTF::BindRepeating(&FileReaderLoader::OnDataPipeReadable,
                           WTF::Unretained(this)));
  }
}
