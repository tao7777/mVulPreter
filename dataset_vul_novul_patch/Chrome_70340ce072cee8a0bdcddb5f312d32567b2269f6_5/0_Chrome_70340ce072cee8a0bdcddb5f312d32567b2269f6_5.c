 bool VaapiVideoDecodeAccelerator::VaapiVP9Accelerator::GetFrameContext(
     const scoped_refptr<VP9Picture>& pic,
     Vp9FrameContext* frame_ctx) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   NOTIMPLEMENTED() << "Frame context update not supported";
   return false;
 }
