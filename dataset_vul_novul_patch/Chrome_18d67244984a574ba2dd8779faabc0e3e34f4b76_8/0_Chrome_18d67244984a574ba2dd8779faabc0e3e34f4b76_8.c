   virtual scoped_refptr<ui::Texture> CreateTransportClient(
       const gfx::Size& size,
       float device_scale_factor,
      const std::string& mailbox_name) {
     if (!shared_context_.get())
         return NULL;
     scoped_refptr<ImageTransportClientTexture> image(
         new ImageTransportClientTexture(shared_context_.get(),
                                         size, device_scale_factor,
                                        mailbox_name));
     return image;
   }
