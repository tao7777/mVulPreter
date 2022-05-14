error::Error GLES2DecoderImpl::HandleGetUniformfv(uint32 immediate_data_size,
                                                  const void* cmd_data) {
  const gles2::cmds::GetUniformfv& c =
      *static_cast<const gles2::cmds::GetUniformfv*>(cmd_data);
  GLuint program = c.program;
  GLint fake_location = c.location;
  GLuint service_id;
  GLint real_location = -1;
  Error error;
   typedef cmds::GetUniformfv::Result Result;
   Result* result;
   GLenum result_type;
  GLsizei result_size;
  if (GetUniformSetup(program, fake_location, c.params_shm_id,
                      c.params_shm_offset, &error, &real_location, &service_id,
                      reinterpret_cast<void**>(&result), &result_type,
                      &result_size)) {
     if (result_type == GL_BOOL || result_type == GL_BOOL_VEC2 ||
         result_type == GL_BOOL_VEC3 || result_type == GL_BOOL_VEC4) {
      GLsizei num_values = result_size / sizeof(Result::Type);
       scoped_ptr<GLint[]> temp(new GLint[num_values]);
       glGetUniformiv(service_id, real_location, temp.get());
       GLfloat* dst = result->GetData();
      for (GLsizei ii = 0; ii < num_values; ++ii) {
        dst[ii] = (temp[ii] != 0);
      }
    } else {
      glGetUniformfv(service_id, real_location, result->GetData());
    }
  }
  return error;
}
