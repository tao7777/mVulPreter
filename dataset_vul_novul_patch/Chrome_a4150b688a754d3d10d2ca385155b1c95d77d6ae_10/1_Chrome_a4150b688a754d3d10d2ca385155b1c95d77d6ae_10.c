WebGLRenderingContextBase::~WebGLRenderingContextBase() {
   destruction_in_progress_ = true;
 
  DestroyContext();

  RestoreEvictedContext(this);
}
