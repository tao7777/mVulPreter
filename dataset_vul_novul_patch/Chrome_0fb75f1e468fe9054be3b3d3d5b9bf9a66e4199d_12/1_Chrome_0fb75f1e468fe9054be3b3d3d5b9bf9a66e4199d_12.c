void Win32StackFrameUnwinder::Win32UnwindFunctions::VirtualUnwind(
    DWORD64 image_base,
    DWORD64 program_counter,
    PRUNTIME_FUNCTION runtime_function,
    CONTEXT* context) {
#ifdef _WIN64
  void* handler_data;
  ULONG64 establisher_frame;
  KNONVOLATILE_CONTEXT_POINTERS nvcontext = {};
  RtlVirtualUnwind(0, image_base, program_counter, runtime_function,
                   context, &handler_data, &establisher_frame, &nvcontext);
#else
  NOTREACHED();
#endif
}
