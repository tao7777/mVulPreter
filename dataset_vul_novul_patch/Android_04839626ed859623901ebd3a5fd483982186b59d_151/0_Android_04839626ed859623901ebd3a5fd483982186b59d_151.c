void EBMLHeader::Init()
long long EBMLHeader::Parse(IMkvReader* pReader, long long& pos) {
  assert(pReader);
 
  long long total, available;
 
