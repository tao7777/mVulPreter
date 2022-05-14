WebGLRenderingContextBase::~WebGLRenderingContextBase() {
   destruction_in_progress_ = true;
 
  clearProgramCompletionQueries();

  DestroyContext();

  RestoreEvictedContext(this);
}
