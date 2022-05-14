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
 
  int64 item_length = static_cast<int64>(item.length());
  if (item_length == -1)
    item_length = result - item.offset();
 
  DCHECK_LT(index, item_length_list_.size());
  item_length_list_[index] = item_length;
  total_size_ += item_length;
 
   if (--pending_get_file_info_count_ == 0)
     DidCountSize(net::OK);
}
