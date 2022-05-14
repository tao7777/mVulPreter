media::mojom::VideoFrameDataPtr MakeVideoFrameData(
    const scoped_refptr<media::VideoFrame>& input) {
  if (input->metadata()->IsTrue(media::VideoFrameMetadata::END_OF_STREAM)) {
    return media::mojom::VideoFrameData::NewEosData(
        media::mojom::EosVideoFrameData::New());
  }

  if (input->storage_type() == media::VideoFrame::STORAGE_MOJO_SHARED_BUFFER) {
     media::MojoSharedBufferVideoFrame* mojo_frame =
         static_cast<media::MojoSharedBufferVideoFrame*>(input.get());
 
    // TODO(https://crbug.com/803136): This should duplicate as READ_ONLY, but
    // can't because there is no guarantee that the input handle is sharable as
    // read-only.
     mojo::ScopedSharedBufferHandle dup = mojo_frame->Handle().Clone(
        mojo::SharedBufferHandle::AccessMode::READ_WRITE);
     DCHECK(dup.is_valid());
 
     return media::mojom::VideoFrameData::NewSharedBufferData(
        media::mojom::SharedBufferVideoFrameData::New(
            std::move(dup), mojo_frame->MappedSize(),
            mojo_frame->stride(media::VideoFrame::kYPlane),
            mojo_frame->stride(media::VideoFrame::kUPlane),
            mojo_frame->stride(media::VideoFrame::kVPlane),
            mojo_frame->PlaneOffset(media::VideoFrame::kYPlane),
            mojo_frame->PlaneOffset(media::VideoFrame::kUPlane),
            mojo_frame->PlaneOffset(media::VideoFrame::kVPlane)));
  }

  if (input->HasTextures()) {
    std::vector<gpu::MailboxHolder> mailbox_holder(
        media::VideoFrame::kMaxPlanes);
    size_t num_planes = media::VideoFrame::NumPlanes(input->format());
    for (size_t i = 0; i < num_planes; i++)
      mailbox_holder[i] = input->mailbox_holder(i);
    return media::mojom::VideoFrameData::NewMailboxData(
        media::mojom::MailboxVideoFrameData::New(std::move(mailbox_holder)));
  }

  NOTREACHED() << "Unsupported VideoFrame conversion";
  return nullptr;
}
