  uint8_t* input() const {
     return input_ + BorderTop() * kOuterBlockSize + BorderLeft();
   }
