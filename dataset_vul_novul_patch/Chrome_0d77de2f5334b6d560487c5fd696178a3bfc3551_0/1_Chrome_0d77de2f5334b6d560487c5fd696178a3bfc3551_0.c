 GLOzone* X11SurfaceFactory::GetGLOzone(gl::GLImplementation implementation) {
   switch (implementation) {
     case gl::kGLImplementationDesktopGL:
       return glx_implementation_.get();
    case gl::kGLImplementationEGLGLES2:
      return egl_implementation_.get();
    default:
      return nullptr;
  }
}
