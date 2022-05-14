 Win32StackFrameUnwinder::Win32StackFrameUnwinder(
    UnwindFunctions* unwind_functions)
     : at_top_frame_(true),
       unwind_info_present_for_all_frames_(true),
      pending_blacklisted_module_(nullptr),
      unwind_functions_(unwind_functions) {
 }
