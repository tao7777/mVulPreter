Track::GetContentEncodingByIndex(unsigned long idx) const {
   const ptrdiff_t count =
       content_encoding_entries_end_ - content_encoding_entries_;
   assert(count >= 0);

 if (idx >= static_cast<unsigned long>(count))
 return NULL;

 return content_encoding_entries_[idx];
}
