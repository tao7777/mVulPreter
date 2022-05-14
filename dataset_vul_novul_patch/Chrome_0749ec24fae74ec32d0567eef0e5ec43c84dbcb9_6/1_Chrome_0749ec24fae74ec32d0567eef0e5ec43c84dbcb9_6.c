HeapObjectHeader* NormalPage::findHeaderFromAddress(Address address) {
  if (address < payload())
    return nullptr;
  if (!m_objectStartBitMapComputed)
    populateObjectStartBitMap();
  size_t objectOffset = address - payload();
  size_t objectStartNumber = objectOffset / allocationGranularity;
  size_t mapIndex = objectStartNumber / 8;
  ASSERT(mapIndex < objectStartBitMapSize);
  size_t bit = objectStartNumber & 7;
  uint8_t byte = m_objectStartBitMap[mapIndex] & ((1 << (bit + 1)) - 1);
  while (!byte) {
    ASSERT(mapIndex > 0);
    byte = m_objectStartBitMap[--mapIndex];
  }
  int leadingZeroes = numberOfLeadingZeroes(byte);
  objectStartNumber = (mapIndex * 8) + 7 - leadingZeroes;
  objectOffset = objectStartNumber * allocationGranularity;
  Address objectAddress = objectOffset + payload();
   HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(objectAddress);
   if (header->isFree())
     return nullptr;
  ASSERT(header->checkHeader());
   return header;
 }
