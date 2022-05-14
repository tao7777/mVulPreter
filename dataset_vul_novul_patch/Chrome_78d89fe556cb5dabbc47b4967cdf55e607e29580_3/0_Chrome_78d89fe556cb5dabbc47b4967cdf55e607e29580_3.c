bool AcceleratedStaticBitmapImage::CopyToTexture(
    gpu::gles2::GLES2Interface* dest_gl,
    GLenum dest_target,
    GLuint dest_texture_id,
    bool unpack_premultiply_alpha,
     bool unpack_flip_y,
     const IntPoint& dest_point,
     const IntRect& source_sub_rectangle) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (!IsValid())
     return false;
  DCHECK(texture_holder_->IsCrossThread() ||
         dest_gl != ContextProviderWrapper()->ContextProvider()->ContextGL());

  EnsureMailbox(kUnverifiedSyncToken, GL_NEAREST);

  dest_gl->WaitSyncTokenCHROMIUM(
      texture_holder_->GetSyncToken().GetConstData());
  GLuint source_texture_id = dest_gl->CreateAndConsumeTextureCHROMIUM(
      texture_holder_->GetMailbox().name);
  dest_gl->CopySubTextureCHROMIUM(
      source_texture_id, 0, dest_target, dest_texture_id, 0, dest_point.X(),
      dest_point.Y(), source_sub_rectangle.X(), source_sub_rectangle.Y(),
      source_sub_rectangle.Width(), source_sub_rectangle.Height(),
      unpack_flip_y ? GL_FALSE : GL_TRUE, GL_FALSE,
      unpack_premultiply_alpha ? GL_FALSE : GL_TRUE);
  dest_gl->DeleteTextures(1, &source_texture_id);

  gpu::SyncToken sync_token;
  dest_gl->GenUnverifiedSyncTokenCHROMIUM(sync_token.GetData());
  texture_holder_->UpdateSyncToken(sync_token);

  return true;
}
