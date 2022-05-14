static bool resolveGLMethods(GraphicsSurfacePrivate* p)
{
     static bool resolved = false;
     if (resolved)
         return true;
#if PLATFORM(QT)
    QOpenGLContext* glContext = p->glContext();
    pGlXBindTexImageEXT = reinterpret_cast<PFNGLXBINDTEXIMAGEEXTPROC>(glContext->getProcAddress("glXBindTexImageEXT"));
    pGlXReleaseTexImageEXT = reinterpret_cast<PFNGLXRELEASETEXIMAGEEXTPROC>(glContext->getProcAddress("glXReleaseTexImageEXT"));
    pGlBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(glContext->getProcAddress("glBindFramebuffer"));
    pGlBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(glContext->getProcAddress("glBlitFramebuffer"));
    pGlGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(glContext->getProcAddress("glGenFramebuffers"));
    pGlDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(glContext->getProcAddress("glDeleteFramebuffers"));
    pGlFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(glContext->getProcAddress("glFramebufferTexture2D"));
#elif PLATFORM(EFL)
     pGlXBindTexImageEXT = reinterpret_cast<PFNGLXBINDTEXIMAGEEXTPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXBindTexImageEXT")));
     pGlXReleaseTexImageEXT = reinterpret_cast<PFNGLXRELEASETEXIMAGEEXTPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXReleaseTexImageEXT")));
     pGlBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glBindFramebuffer")));
    pGlBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glBlitFramebuffer")));

     pGlGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glGenFramebuffers")));
     pGlDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glDeleteFramebuffers")));
     pGlFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glFramebufferTexture2D")));
#endif
     resolved = pGlBlitFramebuffer && pGlBindFramebuffer && pGlXBindTexImageEXT && pGlXReleaseTexImageEXT;
 
     return resolved;
}
