long ContentEncoding::ParseEncryptionEntry(long long start, long long size,
 IMkvReader* pReader,
 ContentEncryption* encryption) {
  assert(pReader);
  assert(encryption);

 long long pos = start;
 const long long stop = start + size;

 while (pos < stop) {
 long long id, size;
 const long status = ParseElementHeader(pReader, pos, stop, id, size);
 if (status < 0) // error
 return status;

 if (id == 0x7E1) {
      encryption->algo = UnserializeUInt(pReader, pos, size);
 if (encryption->algo != 5)

         return E_FILE_FORMAT_INVALID;
     } else if (id == 0x7E2) {
      delete[] encryption -> key_id;
       encryption->key_id = NULL;
       encryption->key_id_len = 0;
 
 if (size <= 0)

         return E_FILE_FORMAT_INVALID;
 
       const size_t buflen = static_cast<size_t>(size);
      typedef unsigned char* buf_t;
      const buf_t buf = new (std::nothrow) unsigned char[buflen];
       if (buf == NULL)
         return -1;
 
 const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
 if (read_status) {
 delete[] buf;
 return status;
 }

      encryption->key_id = buf;

       encryption->key_id_len = buflen;
     } else if (id == 0x7E3) {
      delete[] encryption -> signature;
       encryption->signature = NULL;
       encryption->signature_len = 0;
 
 if (size <= 0)

         return E_FILE_FORMAT_INVALID;
 
       const size_t buflen = static_cast<size_t>(size);
      typedef unsigned char* buf_t;
      const buf_t buf = new (std::nothrow) unsigned char[buflen];
       if (buf == NULL)
         return -1;
 
 const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
 if (read_status) {
 delete[] buf;
 return status;
 }

      encryption->signature = buf;

       encryption->signature_len = buflen;
     } else if (id == 0x7E4) {
      delete[] encryption -> sig_key_id;
       encryption->sig_key_id = NULL;
       encryption->sig_key_id_len = 0;
 
 if (size <= 0)

         return E_FILE_FORMAT_INVALID;
 
       const size_t buflen = static_cast<size_t>(size);
      typedef unsigned char* buf_t;
      const buf_t buf = new (std::nothrow) unsigned char[buflen];
       if (buf == NULL)
         return -1;
 
 const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
 if (read_status) {
 delete[] buf;
 return status;
 }

      encryption->sig_key_id = buf;
      encryption->sig_key_id_len = buflen;
 } else if (id == 0x7E5) {
      encryption->sig_algo = UnserializeUInt(pReader, pos, size);
 } else if (id == 0x7E6) {
      encryption->sig_hash_algo = UnserializeUInt(pReader, pos, size);
 } else if (id == 0x7E7) {
 const long status = ParseContentEncAESSettingsEntry(
          pos, size, pReader, &encryption->aes_settings);
 if (status)
 return status;

     }
 
     pos += size;  // consume payload
    assert(pos <= stop);
   }
 
   return 0;
}
