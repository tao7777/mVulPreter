 static void swizzleImageData(unsigned char* srcAddr,
                             size_t height,
                             size_t bytesPerRow,
                              bool flipY) {
   if (flipY) {
    for (size_t i = 0; i < height / 2; i++) {
      size_t topRowStartPosition = i * bytesPerRow;
      size_t bottomRowStartPosition = (height - 1 - i) * bytesPerRow;
       if (kN32_SkColorType == kBGRA_8888_SkColorType) {  // needs to swizzle
        for (size_t j = 0; j < bytesPerRow; j += 4) {
           std::swap(srcAddr[topRowStartPosition + j],
                     srcAddr[bottomRowStartPosition + j + 2]);
           std::swap(srcAddr[topRowStartPosition + j + 1],
                    srcAddr[bottomRowStartPosition + j + 1]);
          std::swap(srcAddr[topRowStartPosition + j + 2],
                    srcAddr[bottomRowStartPosition + j]);
          std::swap(srcAddr[topRowStartPosition + j + 3],
                    srcAddr[bottomRowStartPosition + j + 3]);
        }
      } else {
        std::swap_ranges(srcAddr + topRowStartPosition,
                         srcAddr + topRowStartPosition + bytesPerRow,
                         srcAddr + bottomRowStartPosition);
      }
     }
   } else {
     if (kN32_SkColorType == kBGRA_8888_SkColorType)  // needs to swizzle
      for (size_t i = 0; i < height * bytesPerRow; i += 4)
         std::swap(srcAddr[i], srcAddr[i + 2]);
   }
 }
