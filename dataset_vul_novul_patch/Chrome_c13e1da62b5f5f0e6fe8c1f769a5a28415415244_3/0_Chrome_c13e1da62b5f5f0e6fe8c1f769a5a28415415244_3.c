  void AddExpectationsForSimulatedAttrib0(

  void AddExpectationsForSimulatedAttrib0WithError(
      GLsizei num_vertices, GLuint buffer_id, GLenum error) {
    if (gfx::GetGLImplementation() == gfx::kGLImplementationEGLGLES2) {
      return;
    }

    EXPECT_CALL(*gl_, GetError())
        .WillOnce(Return(GL_NO_ERROR))
        .WillOnce(Return(error))
        .RetiresOnSaturation();
     EXPECT_CALL(*gl_, BindBuffer(GL_ARRAY_BUFFER, kServiceAttrib0BufferId))
         .Times(1)
         .RetiresOnSaturation();
    EXPECT_CALL(*gl_, BufferData(GL_ARRAY_BUFFER,
                                 num_vertices * sizeof(GLfloat) * 4,
                                  _, GL_DYNAMIC_DRAW))
         .Times(1)
         .RetiresOnSaturation();
    if (error == GL_NO_ERROR) {
      EXPECT_CALL(*gl_, BufferSubData(
          GL_ARRAY_BUFFER, 0, num_vertices * sizeof(GLfloat) * 4, _))
          .Times(1)
          .RetiresOnSaturation();
      EXPECT_CALL(*gl_, VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL))
          .Times(1)
          .RetiresOnSaturation();
      EXPECT_CALL(*gl_, BindBuffer(GL_ARRAY_BUFFER, 0))
          .Times(1)
          .RetiresOnSaturation();
      EXPECT_CALL(*gl_, VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL))
          .Times(1)
          .RetiresOnSaturation();
      EXPECT_CALL(*gl_, BindBuffer(GL_ARRAY_BUFFER, buffer_id))
          .Times(1)
          .RetiresOnSaturation();
    }
  }

  void AddExpectationsForSimulatedAttrib0(
      GLsizei num_vertices, GLuint buffer_id) {
    AddExpectationsForSimulatedAttrib0WithError(
        num_vertices, buffer_id, GL_NO_ERROR);
   }
