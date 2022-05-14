 AudioRendererHost::AudioRendererHost(int render_process_id,
                                      media::AudioManager* audio_manager,
                                     media::AudioSystem* audio_system,
                                      AudioMirroringManager* mirroring_manager,
                                      MediaStreamManager* media_stream_manager,
                                      const std::string& salt)
    : BrowserMessageFilter(AudioMsgStart),
      render_process_id_(render_process_id),
      audio_manager_(audio_manager),
      mirroring_manager_(mirroring_manager),
       media_stream_manager_(media_stream_manager),
       salt_(salt),
       validate_render_frame_id_function_(&ValidateRenderFrameId),
      authorization_handler_(audio_system,
                              media_stream_manager,
                              render_process_id_,
                              salt) {
  DCHECK(audio_manager_);
}
