void UrlData::MergeFrom(const scoped_refptr<UrlData>& other) {
  if (ValidateDataOrigin(other->data_origin_)) {
    DCHECK(thread_checker_.CalledOnValidThread());
    valid_until_ = std::max(valid_until_, other->valid_until_);
    set_length(other->length_);
    cacheable_ |= other->cacheable_;
    range_supported_ |= other->range_supported_;
    if (last_modified_.is_null()) {
       last_modified_ = other->last_modified_;
     }
     bytes_read_from_cache_ += other->bytes_read_from_cache_;
    set_has_opaque_data(other->has_opaque_data_);
     multibuffer()->MergeFrom(other->multibuffer());
   }
 }
