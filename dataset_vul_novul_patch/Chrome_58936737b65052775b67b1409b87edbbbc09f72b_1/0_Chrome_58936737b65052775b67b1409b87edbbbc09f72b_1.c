void BlobURLRequestJob::DidGetFileItemLength(size_t index, int64 result) {
  if (error_)
    return;

  if (result == net::ERR_UPLOAD_FILE_CHANGED) {
    NotifyFailure(net::ERR_FILE_NOT_FOUND);
    return;
  } else if (result < 0) {
    NotifyFailure(result);
    return;
  }

  DCHECK_LT(index, blob_data_->items().size());
   const BlobData::Item& item = blob_data_->items().at(index);
   DCHECK(IsFileType(item.type()));
 
  uint64 file_length = result;
  uint64 item_offset = item.offset();
  uint64 item_length = item.length();

  if (item_offset > file_length) {
    NotifyFailure(net::ERR_FILE_NOT_FOUND);
    return;
  }

  uint64 max_length = file_length - item_offset;

  if (item_length == static_cast<uint64>(-1)) {
    item_length = max_length;
  } else if (item_length > max_length) {
    NotifyFailure(net::ERR_FILE_NOT_FOUND);
    return;
  }
 
  if (!AddItemLength(index, item_length))
    return;
 
   if (--pending_get_file_info_count_ == 0)
     DidCountSize(net::OK);
}
