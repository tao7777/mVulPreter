 void assertObjectHasGCInfo(const void* payload, size_t gcInfoIndex) {
  ASSERT(HeapObjectHeader::fromPayload(payload)->checkHeader());
 #if !defined(COMPONENT_BUILD)
  ASSERT(HeapObjectHeader::fromPayload(payload)->gcInfoIndex() == gcInfoIndex);
#endif
}
