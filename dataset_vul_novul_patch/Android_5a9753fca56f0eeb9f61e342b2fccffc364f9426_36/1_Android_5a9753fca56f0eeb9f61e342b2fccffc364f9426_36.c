  uint8_t* output() const {
     return output_ + BorderTop() * kOuterBlockSize + BorderLeft();
   }
