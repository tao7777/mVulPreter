 static uint64_t ReadBits(BitReader* reader, int num_bits) {
   DCHECK_GE(reader->bits_available(), num_bits);
   DCHECK((num_bits > 0) && (num_bits <= 64));
  uint64_t value;
  reader->ReadBits(num_bits, &value);
   return value;
 }
