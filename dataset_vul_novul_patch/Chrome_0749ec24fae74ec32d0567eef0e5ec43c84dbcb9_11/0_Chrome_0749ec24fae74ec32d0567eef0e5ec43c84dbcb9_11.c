 void HeapObjectHeader::zapMagic() {
  checkHeader();
   m_magic = zappedMagic;
 }
