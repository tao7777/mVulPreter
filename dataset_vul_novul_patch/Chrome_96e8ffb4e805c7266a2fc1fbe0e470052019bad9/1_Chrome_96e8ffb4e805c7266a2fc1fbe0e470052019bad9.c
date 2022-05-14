int FFmpegVideoDecoder::GetVideoBuffer(AVCodecContext* codec_context,
                                       AVFrame* frame) {
  VideoFrame::Format format = PixelFormatToVideoFormat(codec_context->pix_fmt);
  if (format == VideoFrame::UNKNOWN)
    return AVERROR(EINVAL);
  DCHECK(format == VideoFrame::YV12 || format == VideoFrame::YV16 ||
          format == VideoFrame::YV12J);
 
   gfx::Size size(codec_context->width, codec_context->height);
  int ret;
  if ((ret = av_image_check_size(size.width(), size.height(), 0, NULL)) < 0)
     return ret;
 
   gfx::Size natural_size;
  if (codec_context->sample_aspect_ratio.num > 0) {
    natural_size = GetNaturalSize(size,
                                  codec_context->sample_aspect_ratio.num,
                                  codec_context->sample_aspect_ratio.den);
  } else {
     natural_size = config_.natural_size();
   }
 
  if (!VideoFrame::IsValidConfig(format, size, gfx::Rect(size), natural_size))
     return AVERROR(EINVAL);
 
  scoped_refptr<VideoFrame> video_frame =
      frame_pool_.CreateFrame(format, size, gfx::Rect(size),
                              natural_size, kNoTimestamp());
 
   for (int i = 0; i < 3; i++) {
     frame->base[i] = video_frame->data(i);
    frame->data[i] = video_frame->data(i);
    frame->linesize[i] = video_frame->stride(i);
  }

   frame->opaque = NULL;
   video_frame.swap(reinterpret_cast<VideoFrame**>(&frame->opaque));
   frame->type = FF_BUFFER_TYPE_USER;
  frame->width = codec_context->width;
  frame->height = codec_context->height;
   frame->format = codec_context->pix_fmt;
 
   return 0;
}
