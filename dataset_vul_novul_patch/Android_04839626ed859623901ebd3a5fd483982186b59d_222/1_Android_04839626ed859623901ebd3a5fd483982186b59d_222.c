 ContentEncoding::~ContentEncoding() {
   ContentCompression** comp_i = compression_entries_;
 ContentCompression** const comp_j = compression_entries_end_;

 while (comp_i != comp_j) {
 ContentCompression* const comp = *comp_i++;

     delete comp;
   }
 
  delete [] compression_entries_;
 
   ContentEncryption** enc_i = encryption_entries_;
   ContentEncryption** const enc_j = encryption_entries_end_;

 while (enc_i != enc_j) {
 ContentEncryption* const enc = *enc_i++;

     delete enc;
   }
 
  delete [] encryption_entries_;
 }
