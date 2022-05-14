void GLSurfaceEGLSurfaceControl::SwapBuffersAsync(
    SwapCompletionCallback completion_callback,
    PresentationCallback presentation_callback) {
  CommitPendingTransaction(std::move(completion_callback),
                           std::move(presentation_callback));
 }
