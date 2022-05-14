void BindSkiaToInProcessGL() {
  static bool host_StubGL_installed = false;
  if (!host_StubGL_installed) {
    GrGLBinding binding;
    switch (gfx::GetGLImplementation()) {
      case gfx::kGLImplementationNone:
        NOTREACHED();
        return;
      case gfx::kGLImplementationDesktopGL:
        binding = kDesktop_GrGLBinding;
        break;
      case gfx::kGLImplementationOSMesaGL:
        binding = kDesktop_GrGLBinding;
        break;
      case gfx::kGLImplementationEGLGLES2:
        binding = kES2_GrGLBinding;
        break;
       case gfx::kGLImplementationMockGL:
         NOTREACHED();
         return;
      default:
        NOTREACHED();
        return;
     }
 
     static GrGLInterface host_gl_interface = {
      binding,

      kProbe_GrGLCapability,   // NPOTRenderTargetSupport
      kProbe_GrGLCapability,   // MinRenderTargetHeight
      kProbe_GrGLCapability,   // MinRenderTargetWidth

      StubGLActiveTexture,
      StubGLAttachShader,
      StubGLBindAttribLocation,
      StubGLBindBuffer,
      StubGLBindTexture,
      StubGLBlendColor,
      StubGLBlendFunc,
      StubGLBufferData,
      StubGLBufferSubData,
      StubGLClear,
      StubGLClearColor,
      StubGLClearStencil,
      NULL,  // glClientActiveTexture
      NULL,  // glColor4ub
      StubGLColorMask,
      NULL,  // glColorPointer
      StubGLCompileShader,
      StubGLCompressedTexImage2D,
      StubGLCreateProgram,
      StubGLCreateShader,
      StubGLCullFace,
      StubGLDeleteBuffers,
      StubGLDeleteProgram,
      StubGLDeleteShader,
      StubGLDeleteTextures,
      StubGLDepthMask,
      StubGLDisable,
      NULL,  // glDisableClientState
      StubGLDisableVertexAttribArray,
      StubGLDrawArrays,
      StubGLDrawElements,
      StubGLEnable,
      NULL,  // glEnableClientState
      StubGLEnableVertexAttribArray,
      StubGLFrontFace,
      StubGLGenBuffers,
      StubGLGenTextures,
      StubGLGetBufferParameteriv,
      StubGLGetError,
      StubGLGetIntegerv,
      StubGLGetProgramInfoLog,
      StubGLGetProgramiv,
      StubGLGetShaderInfoLog,
      StubGLGetShaderiv,
      StubGLGetString,
      StubGLGetUniformLocation,
      StubGLLineWidth,
      StubGLLinkProgram,
      NULL,  // glLoadMatrixf
      NULL,  // glMatrixMode
      StubGLPixelStorei,
      NULL,  // glPointSize
      StubGLReadPixels,
      StubGLScissor,
      NULL,  // glShadeModel
      StubGLShaderSource,
      StubGLStencilFunc,
      StubGLStencilFuncSeparate,
      StubGLStencilMask,
      StubGLStencilMaskSeparate,
      StubGLStencilOp,
      StubGLStencilOpSeparate,
      NULL,  // glTexCoordPointer
      NULL,  // glTexEnvi
      StubGLTexImage2D,
      StubGLTexParameteri,
      StubGLTexSubImage2D,
      StubGLUniform1f,
      StubGLUniform1i,
      StubGLUniform1fv,
      StubGLUniform1iv,
      StubGLUniform2f,
      StubGLUniform2i,
      StubGLUniform2fv,
      StubGLUniform2iv,
      StubGLUniform3f,
      StubGLUniform3i,
      StubGLUniform3fv,
      StubGLUniform3iv,
      StubGLUniform4f,
      StubGLUniform4i,
      StubGLUniform4fv,
      StubGLUniform4iv,
      StubGLUniformMatrix2fv,
      StubGLUniformMatrix3fv,
      StubGLUniformMatrix4fv,
      StubGLUseProgram,
      StubGLVertexAttrib4fv,
      StubGLVertexAttribPointer,
      NULL,  // glVertexPointer
      StubGLViewport,
      StubGLBindFramebuffer,
      StubGLBindRenderbuffer,
      StubGLCheckFramebufferStatus,
      StubGLDeleteFramebuffers,
      StubGLDeleteRenderbuffers,
      StubGLFramebufferRenderbuffer,
      StubGLFramebufferTexture2D,
      StubGLGenFramebuffers,
      StubGLGenRenderbuffers,
      StubGLRenderBufferStorage,
      StubGLRenderbufferStorageMultisample,
      StubGLBlitFramebuffer,
      NULL,  // glResolveMultisampleFramebuffer
      StubGLMapBuffer,
      StubGLUnmapBuffer,
      NULL,  // glBindFragDataLocationIndexed
      GrGLInterface::kStaticInitEndGuard,
    };
    GrGLSetGLInterface(&host_gl_interface);
    host_StubGL_installed = true;
  }
}
