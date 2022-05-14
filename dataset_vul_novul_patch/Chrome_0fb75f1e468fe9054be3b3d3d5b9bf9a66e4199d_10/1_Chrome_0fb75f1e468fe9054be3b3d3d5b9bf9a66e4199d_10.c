LookupFunctionEntry(DWORD64 program_counter, PDWORD64 image_base) {
#ifdef _WIN64
  return RtlLookupFunctionEntry(program_counter, image_base, nullptr);
#else
  NOTREACHED();
  return nullptr;
#endif
}
