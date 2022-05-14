 void IOHandler::Read(
    const std::string& handle,
    Maybe<int> offset,
    Maybe<int> max_size,
    std::unique_ptr<ReadCallback> callback) {
  static const size_t kDefaultChunkSize = 10 * 1024 * 1024;
  static const char kBlobPrefix[] = "blob:";
 
   scoped_refptr<DevToolsIOContext::ROStream> stream =
       io_context_->GetByHandle(handle);
  if (!stream && process_host_ &&
       StartsWith(handle, kBlobPrefix, base::CompareCase::SENSITIVE)) {
    BrowserContext* browser_context = process_host_->GetBrowserContext();
     ChromeBlobStorageContext* blob_context =
        ChromeBlobStorageContext::GetFor(browser_context);
    StoragePartition* storage_partition = process_host_->GetStoragePartition();
     std::string uuid = handle.substr(strlen(kBlobPrefix));
     stream =
        io_context_->OpenBlob(blob_context, storage_partition, handle, uuid);
   }
 
   if (!stream) {
    callback->sendFailure(Response::InvalidParams("Invalid stream handle"));
    return;
  }
  stream->Read(
      offset.fromMaybe(-1), max_size.fromMaybe(kDefaultChunkSize),
      base::BindOnce(&IOHandler::ReadComplete, weak_factory_.GetWeakPtr(),
                     base::Passed(std::move(callback))));
}
