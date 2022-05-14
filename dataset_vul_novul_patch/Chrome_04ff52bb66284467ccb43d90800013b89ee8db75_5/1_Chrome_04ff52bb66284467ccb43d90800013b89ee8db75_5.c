   MockAudioRendererHost(base::RunLoop* auth_run_loop,
                         int render_process_id,
                         media::AudioManager* audio_manager,
                         AudioMirroringManager* mirroring_manager,
                         MediaStreamManager* media_stream_manager,
                         const std::string& salt)
       : AudioRendererHost(render_process_id,
                           audio_manager,
                           mirroring_manager,
                           media_stream_manager,
                           salt),
        shared_memory_length_(0),
        auth_run_loop_(auth_run_loop) {
    set_render_frame_id_validate_function_for_testing(&ValidateRenderFrameId);
  }
