 ContentEncoding::ContentEncryption::~ContentEncryption() {
  delete [] key_id;
  delete [] signature;
  delete [] sig_key_id;
 }
