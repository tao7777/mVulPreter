 void GLSurfaceEGLSurfaceControl::CommitOverlayPlanesAsync(
     SwapCompletionCallback completion_callback,
     PresentationCallback presentation_callback) {
  CommitPendingTransaction(window_rect_, std::move(completion_callback),
                           std::move(presentation_callback));
}

void GLSurfaceEGLSurfaceControl::PostSubBufferAsync(
    int x,
    int y,
    int width,
    int height,
    SwapCompletionCallback completion_callback,
    PresentationCallback presentation_callback) {
  CommitPendingTransaction(gfx::Rect(x, y, width, height),
                           std::move(completion_callback),
                            std::move(presentation_callback));
 }
