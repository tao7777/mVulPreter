 void DelegatedFrameHost::CopyFromCompositingSurface(
     const gfx::Rect& src_subrect,
    const gfx::Size& output_size,
     const base::Callback<void(bool, const SkBitmap&)>& callback,
     const SkColorType color_type) {
  bool format_support = ((color_type == kRGB_565_SkColorType) ||
                         (color_type == kN32_SkColorType));
  DCHECK(format_support);
  if (!CanCopyToBitmap()) {
    callback.Run(false, SkBitmap());
     return;
   }
 
   scoped_ptr<cc::CopyOutputRequest> request =
       cc::CopyOutputRequest::CreateRequest(base::Bind(
           &DelegatedFrameHost::CopyFromCompositingSurfaceHasResult,
          output_size,
           color_type,
           callback));
  request->set_area(src_subrect);
   client_->RequestCopyOfOutput(request.Pass());
 }
