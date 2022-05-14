ContentEncoding::GetEncryptionByIndex(unsigned long idx) const {
const ContentEncoding::ContentEncryption* ContentEncoding::GetEncryptionByIndex(
    unsigned long idx) const {
   const ptrdiff_t count = encryption_entries_end_ - encryption_entries_;
   assert(count >= 0);
 
 if (idx >= static_cast<unsigned long>(count))
 return NULL;

 return encryption_entries_[idx];
}
