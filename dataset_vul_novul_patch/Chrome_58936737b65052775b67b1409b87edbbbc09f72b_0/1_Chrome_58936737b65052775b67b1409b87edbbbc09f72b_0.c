 void BlobURLRequestJob::CountSize() {
   error_ = false;
   pending_get_file_info_count_ = 0;
  total_size_ = 0;
  item_length_list_.resize(blob_data_->items().size());

  for (size_t i = 0; i < blob_data_->items().size(); ++i) {
    const BlobData::Item& item = blob_data_->items().at(i);
    if (IsFileType(item.type())) {
      ++pending_get_file_info_count_;
      GetFileStreamReader(i)->GetLength(
          base::Bind(&BlobURLRequestJob::DidGetFileItemLength,
                      weak_factory_.GetWeakPtr(), i));
       continue;
     }
    int64 item_length = static_cast<int64>(item.length());
    item_length_list_[i] = item_length;
    total_size_ += item_length;
   }
 
   if (pending_get_file_info_count_ == 0)
    DidCountSize(net::OK);
}
