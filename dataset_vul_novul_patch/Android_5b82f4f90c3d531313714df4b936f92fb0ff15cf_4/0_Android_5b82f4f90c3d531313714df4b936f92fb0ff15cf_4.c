omx_vdec::~omx_vdec()
{
    m_pmem_info = NULL;
 struct v4l2_decoder_cmd dec;
    DEBUG_PRINT_HIGH("In OMX vdec Destructor");
 if (m_pipe_in) close(m_pipe_in);
 if (m_pipe_out) close(m_pipe_out);
    m_pipe_in = -1;
    m_pipe_out = -1;
    DEBUG_PRINT_HIGH("Waiting on OMX Msg Thread exit");
 if (msg_thread_created)
        pthread_join(msg_thread_id,NULL);
    DEBUG_PRINT_HIGH("Waiting on OMX Async Thread exit");
    dec.cmd = V4L2_DEC_CMD_STOP;
 if (drv_ctx.video_driver_fd >=0 ) {
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_DECODER_CMD, &dec))
            DEBUG_PRINT_ERROR("STOP Command failed");
 }
 if (async_thread_created)
        pthread_join(async_thread_id,NULL);
    unsubscribe_to_events(drv_ctx.video_driver_fd);

     close(drv_ctx.video_driver_fd);
     pthread_mutex_destroy(&m_lock);
     pthread_mutex_destroy(&c_lock);
    pthread_mutex_destroy(&buf_lock);
     sem_destroy(&m_cmd_lock);
     if (perf_flag) {
         DEBUG_PRINT_HIGH("--> TOTAL PROCESSING TIME");
        dec_time.end();
 }
    DEBUG_PRINT_INFO("Exit OMX vdec Destructor: fd=%d",drv_ctx.video_driver_fd);
}
