static bool CheckMov(const uint8* buffer, int buffer_size) {
  RCHECK(buffer_size > 8);
 
   int offset = 0;
   while (offset + 8 < buffer_size) {
    uint32 atomsize = Read32(buffer + offset);
     uint32 atomtype = Read32(buffer + offset + 4);
     switch (atomtype) {
      case TAG('f','t','y','p'):
      case TAG('p','d','i','n'):
      case TAG('m','o','o','v'):
      case TAG('m','o','o','f'):
      case TAG('m','f','r','a'):
      case TAG('m','d','a','t'):
      case TAG('f','r','e','e'):
      case TAG('s','k','i','p'):
      case TAG('m','e','t','a'):
      case TAG('m','e','c','o'):
      case TAG('s','t','y','p'):
      case TAG('s','i','d','x'):
      case TAG('s','s','i','x'):
      case TAG('p','r','f','t'):
      case TAG('b','l','o','c'):
        break;
      default:
        return false;
    }
    if (atomsize == 1) {
      if (offset + 16 > buffer_size)
        break;
      if (Read32(buffer + offset + 8) != 0)
         break;  // Offset is way past buffer size.
       atomsize = Read32(buffer + offset + 12);
     }
    if (atomsize == 0 || atomsize > static_cast<size_t>(buffer_size))
       break;  // Indicates the last atom or length too big.
     offset += atomsize;
   }
  return true;
}
