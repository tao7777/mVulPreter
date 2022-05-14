 Win32StackFrameUnwinder::Win32StackFrameUnwinder(
    scoped_ptr<UnwindFunctions> unwind_functions)
     : at_top_frame_(true),
       unwind_info_present_for_all_frames_(true),
      unwind_functions_(unwind_functions.Pass()) {
 }
