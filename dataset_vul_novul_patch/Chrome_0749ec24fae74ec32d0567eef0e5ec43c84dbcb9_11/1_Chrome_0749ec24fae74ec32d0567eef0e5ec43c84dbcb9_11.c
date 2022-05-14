 void HeapObjectHeader::zapMagic() {
  ASSERT(checkHeader());
   m_magic = zappedMagic;
 }
