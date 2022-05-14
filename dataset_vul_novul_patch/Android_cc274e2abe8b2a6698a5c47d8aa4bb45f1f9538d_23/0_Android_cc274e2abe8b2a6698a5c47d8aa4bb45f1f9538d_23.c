void mkvparser::GetVersion(int& major, int& minor, int& build, int& revision) {
IMkvReader::~IMkvReader() {}

template<typename Type> Type* SafeArrayAlloc(unsigned long long num_elements,
                                             unsigned long long element_size) {
  if (num_elements == 0 || element_size == 0)
    return NULL;

  const size_t kMaxAllocSize = 0x80000000;  // 2GiB
  const unsigned long long num_bytes = num_elements * element_size;
  if (element_size > (kMaxAllocSize / num_elements))
    return NULL;

  return new (std::nothrow) Type[num_bytes];
}

void GetVersion(int& major, int& minor, int& build, int& revision) {
   major = 1;
   minor = 0;
   build = 0;
  revision = 30;
 }
