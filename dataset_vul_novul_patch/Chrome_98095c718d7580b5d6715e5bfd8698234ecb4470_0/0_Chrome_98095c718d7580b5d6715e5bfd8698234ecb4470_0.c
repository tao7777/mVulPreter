 void WebGL2RenderingContextBase::bindVertexArray(
     WebGLVertexArrayObject* vertex_array) {
  bool deleted;
  if (!CheckObjectToBeBound("bindVertexArray", vertex_array, deleted))
     return;
  if (deleted) {
     SynthesizeGLError(GL_INVALID_OPERATION, "bindVertexArray",
                      "attempt to bind a deleted vertex array");
     return;
   }
 
  if (vertex_array && !vertex_array->IsDefaultObject() &&
      vertex_array->Object()) {
    ContextGL()->BindVertexArrayOES(ObjectOrZero(vertex_array));

    vertex_array->SetHasEverBeenBound();
    SetBoundVertexArrayObject(vertex_array);
  } else {
    ContextGL()->BindVertexArrayOES(0);
    SetBoundVertexArrayObject(nullptr);
  }
}
