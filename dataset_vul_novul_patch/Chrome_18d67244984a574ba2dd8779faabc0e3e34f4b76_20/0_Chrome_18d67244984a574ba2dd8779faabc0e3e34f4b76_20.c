 void RenderWidgetHostViewAura::AcceleratedSurfaceNew(
       int32 width_in_pixel,
       int32 height_in_pixel,
      uint64 surface_handle,
      const std::string& mailbox_name) {
   ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
   scoped_refptr<ui::Texture> surface(factory->CreateTransportClient(
       gfx::Size(width_in_pixel, height_in_pixel), device_scale_factor_,
      mailbox_name));
   if (!surface) {
     LOG(ERROR) << "Failed to create ImageTransport texture";
     return;
   }
   image_transport_clients_[surface_handle] = surface;
 }
