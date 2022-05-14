  void OnImageDecoded(const gfx::Image& decoded_image) {
    image_decoded_callback_.Run(decoded_image.AsBitmap());
    delete this;
  }
