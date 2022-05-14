void DrawingBuffer::ReadBackFramebuffer(unsigned char* pixels,
                                        int width,
                                        int height,
                                         ReadbackOrder readback_order,
                                         WebGLImageConversion::AlphaOp op) {
   DCHECK(state_restorer_);
  state_restorer_->SetPixelPackAlignmentDirty();
   gl_->PixelStorei(GL_PACK_ALIGNMENT, 1);
   gl_->ReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
 
   size_t buffer_size = 4 * width * height;

  if (readback_order == kReadbackSkia) {
#if (SK_R32_SHIFT == 16) && !SK_B32_SHIFT
    for (size_t i = 0; i < buffer_size; i += 4) {
      std::swap(pixels[i], pixels[i + 2]);
    }
#endif
  }

  if (op == WebGLImageConversion::kAlphaDoPremultiply) {
    for (size_t i = 0; i < buffer_size; i += 4) {
      pixels[i + 0] = std::min(255, pixels[i + 0] * pixels[i + 3] / 255);
      pixels[i + 1] = std::min(255, pixels[i + 1] * pixels[i + 3] / 255);
      pixels[i + 2] = std::min(255, pixels[i + 2] * pixels[i + 3] / 255);
    }
  } else if (op != WebGLImageConversion::kAlphaDoNothing) {
    NOTREACHED();
  }
}
