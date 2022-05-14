void TransportTexture::OnTexturesCreated(std::vector<int> textures) {
   bool ret = decoder_->MakeCurrent();
   if (!ret) {
     LOG(ERROR) << "Failed to switch context";
    return;
  }

  output_textures_->clear();
  for (size_t i = 0; i < textures.size(); ++i) {
    uint32 gl_texture = 0;

    ret = decoder_->GetServiceTextureId(textures[i], &gl_texture);
    DCHECK(ret) << "Cannot translate client texture ID to service ID";
    output_textures_->push_back(gl_texture);
    texture_map_.insert(std::make_pair(gl_texture, textures[i]));
  }

  create_task_->Run();
  create_task_.reset();
  output_textures_ = NULL;
}
