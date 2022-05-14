size_t jsuGetFreeStack() {
#ifdef ARM
  void *frame = __builtin_frame_address(0);
  size_t stackPos = (size_t)((char*)frame);
  size_t stackEnd = (size_t)((char*)&LINKER_END_VAR);
  if (stackPos < stackEnd) return 0; // should never happen, but just in case of overflow!
  return  stackPos - stackEnd;
#elif defined(LINUX)
   char ptr; // this is on the stack
   extern void *STACK_BASE;
   uint32_t count =  (uint32_t)((size_t)STACK_BASE - (size_t)&ptr);
  const uint32_t max_stack = 1000000; // give it 1 megabyte of stack
  if (count>max_stack) return 0;
  return max_stack - count;
 #else
   return 1000000; // no stack depth check on this platform
#endif
}
