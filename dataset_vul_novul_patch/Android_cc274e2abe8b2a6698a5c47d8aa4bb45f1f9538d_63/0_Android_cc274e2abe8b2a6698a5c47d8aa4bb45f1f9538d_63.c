long mkvparser::UnserializeFloat(IMkvReader* pReader, long long pos,
long UnserializeFloat(IMkvReader* pReader, long long pos, long long size_,
                      double& result) {
  if (!pReader || pos < 0 || ((size_ != 4) && (size_ != 8)))
     return E_FILE_FORMAT_INVALID;
 
   const long size = static_cast<long>(size_);

 unsigned char buf[8];

 const int status = pReader->Read(pos, size, buf);

 if (status < 0) // error
 return status;

 if (size == 4) {
 union {
 float f;
 unsigned long ff;
 };

    ff = 0;

 for (int i = 0;;) {
      ff |= buf[i];

 if (++i >= 4)
 break;

      ff <<= 8;
 }

 
     result = f;
   } else {
     union {
       double d;
       unsigned long long dd;
 };

    dd = 0;

 for (int i = 0;;) {
      dd |= buf[i];

 if (++i >= 8)
 break;

      dd <<= 8;
 }


     result = d;
   }
 
  if (std::isinf(result) || std::isnan(result))
    return E_FILE_FORMAT_INVALID;

   return 0;
 }
