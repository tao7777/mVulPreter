 void WebGL2RenderingContextBase::bindVertexArray(
     WebGLVertexArrayObject* vertex_array) {
  if (isContextLost())
     return;
  if (vertex_array &&
      (vertex_array->IsDeleted() || !vertex_array->Validate(nullptr, this))) {
     SynthesizeGLError(GL_INVALID_OPERATION, "bindVertexArray",
                      "invalid vertexArray");
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
