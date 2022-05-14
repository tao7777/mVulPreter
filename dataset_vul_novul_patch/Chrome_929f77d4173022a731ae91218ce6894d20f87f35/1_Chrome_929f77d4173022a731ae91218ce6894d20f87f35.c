bool BitReaderCore::ReadBitsInternal(int num_bits, uint64_t* out) {
  DCHECK_GE(num_bits, 0);

  if (num_bits == 0) {
    *out = 0;
    return true;
  }

  if (num_bits > nbits_ && !Refill(num_bits)) {
     nbits_ = 0;
     reg_ = 0;
     return false;
   }
 
  bits_read_ += num_bits;

  if (num_bits == kRegWidthInBits) {
    *out = reg_;
    reg_ = 0;
    nbits_ = 0;
    return true;
  }

  *out = reg_ >> (kRegWidthInBits - num_bits);
  reg_ <<= num_bits;
  nbits_ -= num_bits;
  return true;
}
