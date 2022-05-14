int RecordStack(CONTEXT* context,
int RecordStack(CONTEXT* context, int max_stack_size,
                const void* instruction_pointers[]) {
 #ifdef _WIN64
  Win32StackFrameUnwinder frame_unwinder;
   int i = 0;
   for (; (i < max_stack_size) && context->Rip; ++i) {
     instruction_pointers[i] = reinterpret_cast<const void*>(context->Rip);
    if (!frame_unwinder.TryUnwind(context))
       return i + 1;
   }
   return i;
#else
  return 0;
#endif
}
