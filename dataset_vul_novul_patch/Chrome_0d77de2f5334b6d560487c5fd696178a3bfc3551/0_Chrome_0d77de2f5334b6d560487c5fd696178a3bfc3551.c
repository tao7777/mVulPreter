 X11SurfaceFactory::GetAllowedGLImplementations() {
  DCHECK(thread_checker_.CalledOnValidThread());
   std::vector<gl::GLImplementation> impls;
   impls.push_back(gl::kGLImplementationEGLGLES2);
  impls.push_back(gl::kGLImplementationDesktopGL);
  impls.push_back(gl::kGLImplementationOSMesaGL);
  return impls;
 }
