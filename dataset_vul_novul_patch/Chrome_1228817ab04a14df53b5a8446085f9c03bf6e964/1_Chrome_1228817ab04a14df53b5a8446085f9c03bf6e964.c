 void DelegatedFrameHost::CopyFromCompositingSurface(
     const gfx::Rect& src_subrect,
    const gfx::Size& dst_size,
     const base::Callback<void(bool, const SkBitmap&)>& callback,
     const SkColorType color_type) {
  bool format_support = ((color_type == kRGB_565_SkColorType) ||
                         (color_type == kN32_SkColorType));
  DCHECK(format_support);
  if (!CanCopyToBitmap()) {
    callback.Run(false, SkBitmap());
     return;
   }
 
  const gfx::Size& dst_size_in_pixel =
      client_->ConvertViewSizeToPixel(dst_size);
   scoped_ptr<cc::CopyOutputRequest> request =
       cc::CopyOutputRequest::CreateRequest(base::Bind(
           &DelegatedFrameHost::CopyFromCompositingSurfaceHasResult,
          dst_size_in_pixel,
           color_type,
           callback));
  gfx::Rect src_subrect_in_pixel =
      ConvertRectToPixel(client_->CurrentDeviceScaleFactor(), src_subrect);
  request->set_area(src_subrect_in_pixel);
   client_->RequestCopyOfOutput(request.Pass());
 }
