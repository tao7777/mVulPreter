 void VaapiVideoDecodeAccelerator::VaapiH264Accelerator::FillVAPicture(
     VAPictureH264* va_pic,
     scoped_refptr<H264Picture> pic) {
   VASurfaceID va_surface_id = VA_INVALID_SURFACE;
 
   if (!pic->nonexisting) {
    scoped_refptr<VaapiDecodeSurface> dec_surface =
        H264PictureToVaapiDecodeSurface(pic);
    va_surface_id = dec_surface->va_surface()->id();
  }

  va_pic->picture_id = va_surface_id;
  va_pic->frame_idx = pic->frame_num;
  va_pic->flags = 0;

  switch (pic->field) {
    case H264Picture::FIELD_NONE:
      break;
    case H264Picture::FIELD_TOP:
      va_pic->flags |= VA_PICTURE_H264_TOP_FIELD;
      break;
    case H264Picture::FIELD_BOTTOM:
      va_pic->flags |= VA_PICTURE_H264_BOTTOM_FIELD;
      break;
  }

  if (pic->ref) {
    va_pic->flags |= pic->long_term ? VA_PICTURE_H264_LONG_TERM_REFERENCE
                                    : VA_PICTURE_H264_SHORT_TERM_REFERENCE;
  }

  va_pic->TopFieldOrderCnt = pic->top_field_order_cnt;
  va_pic->BottomFieldOrderCnt = pic->bottom_field_order_cnt;
}
