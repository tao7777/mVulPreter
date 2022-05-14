 AcceleratedStaticBitmapImage::AcceleratedStaticBitmapImage(
    const gpu::Mailbox& mailbox,
    const gpu::SyncToken& sync_token,
    unsigned texture_id,
    base::WeakPtr<WebGraphicsContext3DProviderWrapper>&&
        context_provider_wrapper,
    IntSize mailbox_size)
    : paint_image_content_id_(cc::PaintImage::GetNextContentId()) {
   texture_holder_ = std::make_unique<MailboxTextureHolder>(
       mailbox, sync_token, texture_id, std::move(context_provider_wrapper),
       mailbox_size);
 }
