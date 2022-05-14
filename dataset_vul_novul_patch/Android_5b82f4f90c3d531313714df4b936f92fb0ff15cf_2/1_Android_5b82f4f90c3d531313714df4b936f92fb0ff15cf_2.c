OMX_ERRORTYPE omx_vdec::free_input_buffer(OMX_BUFFERHEADERTYPE *bufferHdr)
{
 unsigned int index = 0;
 if (bufferHdr == NULL || m_inp_mem_ptr == NULL) {
 return OMX_ErrorBadParameter;
 }


     index = bufferHdr - m_inp_mem_ptr;
     DEBUG_PRINT_LOW("Free Input Buffer index = %d",index);
 
     if (index < drv_ctx.ip_buf.actualcount && drv_ctx.ptr_inputbuffer) {
         DEBUG_PRINT_LOW("Free Input Buffer index = %d",index);
         if (drv_ctx.ptr_inputbuffer[index].pmem_fd > 0) {
 struct vdec_setbuffer_cmd setbuffers;
            setbuffers.buffer_type = VDEC_BUFFER_TYPE_INPUT;
            memcpy (&setbuffers.buffer,&drv_ctx.ptr_inputbuffer[index],
 sizeof (vdec_bufferpayload));
 if (!secure_mode) {
                DEBUG_PRINT_LOW("unmap the input buffer fd=%d",
                        drv_ctx.ptr_inputbuffer[index].pmem_fd);
                DEBUG_PRINT_LOW("unmap the input buffer size=%u  address = %p",
 (unsigned int)drv_ctx.ptr_inputbuffer[index].mmaped_size,
                        drv_ctx.ptr_inputbuffer[index].bufferaddr);
                munmap (drv_ctx.ptr_inputbuffer[index].bufferaddr,
                        drv_ctx.ptr_inputbuffer[index].mmaped_size);
 }
            close (drv_ctx.ptr_inputbuffer[index].pmem_fd);
            drv_ctx.ptr_inputbuffer[index].pmem_fd = -1;
 if (m_desc_buffer_ptr && m_desc_buffer_ptr[index].buf_addr) {
                free(m_desc_buffer_ptr[index].buf_addr);
                m_desc_buffer_ptr[index].buf_addr = NULL;
                m_desc_buffer_ptr[index].desc_data_size = 0;
 }
#ifdef USE_ION
            free_ion_memory(&drv_ctx.ip_buf_ion_info[index]);
#endif
 }
 }

 return OMX_ErrorNone;
}
