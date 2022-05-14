 GLboolean WebGL2RenderingContextBase::isTransformFeedback(
     WebGLTransformFeedback* feedback) {
  if (isContextLost() || !feedback)
     return 0;
 
   if (!feedback->HasEverBeenBound())
    return 0;

  return ContextGL()->IsTransformFeedback(feedback->Object());
}
