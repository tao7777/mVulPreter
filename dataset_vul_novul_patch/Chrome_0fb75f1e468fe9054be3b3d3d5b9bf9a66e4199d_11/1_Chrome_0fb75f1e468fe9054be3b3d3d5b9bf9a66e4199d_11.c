 bool Win32StackFrameUnwinder::TryUnwind(CONTEXT* context) {
 #ifdef _WIN64
   CHECK(!at_top_frame_ || unwind_info_present_for_all_frames_);
  CHECK(!pending_blacklisted_module_);
 
   ULONG64 image_base;
  PRUNTIME_FUNCTION runtime_function =
      unwind_functions_->LookupFunctionEntry(context->Rip, &image_base);

  if (runtime_function) {
    unwind_functions_->VirtualUnwind(image_base, context->Rip, runtime_function,
                                     context);
    at_top_frame_ = false;
  } else {

    if (at_top_frame_) {
      at_top_frame_ = false;

      if (LeafUnwindBlacklist::GetInstance()->IsBlacklisted(
              reinterpret_cast<const void*>(image_base))) {
        return false;
      }

      context->Rip = context->Rsp;
      context->Rsp += 8;
      unwind_info_present_for_all_frames_ = false;
    } else {
       if (unwind_info_present_for_all_frames_) {
        pending_blacklisted_module_ =
            reinterpret_cast<const void *>(image_base);
       } else {
      }
      return false;
    }
  }

  return true;
#else
  NOTREACHED();
  return false;
#endif
 }
