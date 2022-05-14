error::Error GLES2DecoderImpl::HandleGetUniformiv(uint32 immediate_data_size,
                                                  const void* cmd_data) {
  const gles2::cmds::GetUniformiv& c =
      *static_cast<const gles2::cmds::GetUniformiv*>(cmd_data);
  GLuint program = c.program;
   GLint fake_location = c.location;
   GLuint service_id;
   GLenum result_type;
  GLsizei result_size;
   GLint real_location = -1;
   Error error;
   void* result;
  if (GetUniformSetup(program, fake_location, c.params_shm_id,
                      c.params_shm_offset, &error, &real_location, &service_id,
                      &result, &result_type, &result_size)) {
     glGetUniformiv(
         service_id, real_location,
         static_cast<cmds::GetUniformiv::Result*>(result)->GetData());
  }
  return error;
}
