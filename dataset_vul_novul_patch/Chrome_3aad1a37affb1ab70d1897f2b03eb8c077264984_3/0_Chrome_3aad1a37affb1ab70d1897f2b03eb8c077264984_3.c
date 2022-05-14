bool TextureManager::TextureInfo::ValidForTexture(
    GLint face,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type) const {
  size_t face_index = GLTargetToFaceIndex(face);
   if (level >= 0 && face_index < level_infos_.size() &&
       static_cast<size_t>(level) < level_infos_[face_index].size()) {
     const LevelInfo& info = level_infos_[GLTargetToFaceIndex(face)][level];
    int32 right;
    int32 top;
    return SafeAddInt32(xoffset, width, &right) &&
           SafeAddInt32(yoffset, height, &top) &&
            xoffset >= 0 &&
            yoffset >= 0 &&
            right <= info.width &&
           top <= info.height &&
           format == info.internal_format &&
           type == info.type;
  }
  return false;
}
