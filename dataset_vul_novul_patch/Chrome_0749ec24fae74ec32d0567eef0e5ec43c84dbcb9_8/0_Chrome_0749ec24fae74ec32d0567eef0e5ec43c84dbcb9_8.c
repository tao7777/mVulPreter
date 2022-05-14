size_t NormalPage::objectPayloadSizeForTesting() {
  size_t objectPayloadSize = 0;
  Address headerAddress = payload();
  markAsSwept();
  ASSERT(headerAddress != payloadEnd());
  do {
     HeapObjectHeader* header =
         reinterpret_cast<HeapObjectHeader*>(headerAddress);
     if (!header->isFree()) {
      header->checkHeader();
       objectPayloadSize += header->payloadSize();
     }
     ASSERT(header->size() < blinkPagePayloadSize());
    headerAddress += header->size();
    ASSERT(headerAddress <= payloadEnd());
  } while (headerAddress < payloadEnd());
  return objectPayloadSize;
}
