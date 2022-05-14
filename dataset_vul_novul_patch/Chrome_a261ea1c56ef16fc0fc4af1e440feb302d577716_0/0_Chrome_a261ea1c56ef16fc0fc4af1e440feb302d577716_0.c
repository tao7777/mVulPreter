 void FileReaderLoader::OnCalculatedSize(uint64_t total_size,
                                         uint64_t expected_content_size) {
  auto weak_this = weak_factory_.GetWeakPtr();
   OnStartLoading(expected_content_size);
  // OnStartLoading calls out to our client, which could delete |this|, so bail
  // out if that happened.
  if (!weak_this)
    return;

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
