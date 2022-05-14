RenderWidgetHostViewAndroid::~RenderWidgetHostViewAndroid() {
  SetContentViewCore(NULL);
  if (!shared_surface_.is_null()) {
     ImageTransportFactoryAndroid::GetInstance()->DestroySharedSurfaceHandle(
         shared_surface_);
   }
  if (texture_id_in_layer_) {
    ImageTransportFactoryAndroid::GetInstance()->DeleteTexture(
        texture_id_in_layer_);
  }
 }
