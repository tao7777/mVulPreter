long ContentEncoding::ParseContentEncAESSettingsEntry(
 long long start, long long size, IMkvReader* pReader,
 ContentEncAESSettings* aes) {
  assert(pReader);
  assert(aes);

 long long pos = start;
 const long long stop = start + size;

 while (pos < stop) {
 long long id, size;
 const long status = ParseElementHeader(pReader, pos, stop, id, size);
 if (status < 0) // error
 return status;

 if (id == 0x7E8) {
      aes->cipher_mode = UnserializeUInt(pReader, pos, size);
 if (aes->cipher_mode != 1)
 return E_FILE_FORMAT_INVALID;

     }
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
   return 0;
}
