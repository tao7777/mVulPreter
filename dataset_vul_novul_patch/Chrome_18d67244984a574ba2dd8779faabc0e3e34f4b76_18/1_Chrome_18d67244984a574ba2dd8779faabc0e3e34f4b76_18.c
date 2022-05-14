RenderWidgetHostViewAndroid::~RenderWidgetHostViewAndroid() {
  SetContentViewCore(NULL);
  if (!shared_surface_.is_null()) {
     ImageTransportFactoryAndroid::GetInstance()->DestroySharedSurfaceHandle(
         shared_surface_);
   }
 }
