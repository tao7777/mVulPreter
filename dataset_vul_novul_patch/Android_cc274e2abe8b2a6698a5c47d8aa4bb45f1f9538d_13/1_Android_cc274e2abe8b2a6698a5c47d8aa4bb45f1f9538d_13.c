ContentEncoding::GetCompressionByIndex(unsigned long idx) const {
   const ptrdiff_t count = compression_entries_end_ - compression_entries_;
   assert(count >= 0);
 
 if (idx >= static_cast<unsigned long>(count))
 return NULL;

 return compression_entries_[idx];
}
