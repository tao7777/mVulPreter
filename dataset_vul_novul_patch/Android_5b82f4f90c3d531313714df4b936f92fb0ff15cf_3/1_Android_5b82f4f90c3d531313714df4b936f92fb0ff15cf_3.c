omx_vdec::omx_vdec(): m_error_propogated(false),
    m_state(OMX_StateInvalid),
    m_app_data(NULL),
    m_inp_mem_ptr(NULL),
    m_out_mem_ptr(NULL),
    input_flush_progress (false),
    output_flush_progress (false),
    input_use_buffer (false),
    output_use_buffer (false),
    ouput_egl_buffers(false),
    m_use_output_pmem(OMX_FALSE),
    m_out_mem_region_smi(OMX_FALSE),
    m_out_pvt_entry_pmem(OMX_FALSE),
    pending_input_buffers(0),
    pending_output_buffers(0),
    m_out_bm_count(0),
    m_inp_bm_count(0),
    m_inp_bPopulated(OMX_FALSE),
    m_out_bPopulated(OMX_FALSE),
    m_flags(0),
#ifdef _ANDROID_
    m_heap_ptr(NULL),
#endif
    m_inp_bEnabled(OMX_TRUE),
    m_out_bEnabled(OMX_TRUE),
    m_in_alloc_cnt(0),
    m_platform_list(NULL),
    m_platform_entry(NULL),
    m_pmem_info(NULL),
    h264_parser(NULL),
    arbitrary_bytes (true),
    psource_frame (NULL),
    pdest_frame (NULL),
    m_inp_heap_ptr (NULL),
    m_phdr_pmem_ptr(NULL),
    m_heap_inp_bm_count (0),
    codec_type_parse ((codec_type)0),
    first_frame_meta (true),
    frame_count (0),
    nal_count (0),
    nal_length(0),
    look_ahead_nal (false),
    first_frame(0),
    first_buffer(NULL),
    first_frame_size (0),
    m_device_file_ptr(NULL),
    m_vc1_profile((vc1_profile_type)0),
    h264_last_au_ts(LLONG_MAX),
    h264_last_au_flags(0),
    m_disp_hor_size(0),
    m_disp_vert_size(0),
    prev_ts(LLONG_MAX),
    rst_prev_ts(true),
    frm_int(0),
    in_reconfig(false),
    m_display_id(NULL),
    client_extradata(0),
    m_reject_avc_1080p_mp (0),
#ifdef _ANDROID_
    m_enable_android_native_buffers(OMX_FALSE),
    m_use_android_native_buffers(OMX_FALSE),
    iDivXDrmDecrypt(NULL),
#endif
    m_desc_buffer_ptr(NULL),
    secure_mode(false),
    m_other_extradata(NULL),
    m_profile(0),
    client_set_fps(false),
    m_last_rendered_TS(-1),
    m_queued_codec_config_count(0),
    secure_scaling_to_non_secure_opb(false)
{
 /* Assumption is that , to begin with , we have all the frames with decoder */
    DEBUG_PRINT_HIGH("In %u bit OMX vdec Constructor", (unsigned int)sizeof(long) * 8);
    memset(&m_debug,0,sizeof(m_debug));
#ifdef _ANDROID_
 char property_value[PROPERTY_VALUE_MAX] = {0};
    property_get("vidc.debug.level", property_value, "1");
    debug_level = atoi(property_value);
    property_value[0] = '\0';

    DEBUG_PRINT_HIGH("In OMX vdec Constructor");

    property_get("vidc.dec.debug.perf", property_value, "0");
    perf_flag = atoi(property_value);
 if (perf_flag) {
        DEBUG_PRINT_HIGH("vidc.dec.debug.perf is %d", perf_flag);
        dec_time.start();
        proc_frms = latency = 0;
 }
    prev_n_filled_len = 0;
    property_value[0] = '\0';
    property_get("vidc.dec.debug.ts", property_value, "0");
    m_debug_timestamp = atoi(property_value);
    DEBUG_PRINT_HIGH("vidc.dec.debug.ts value is %d",m_debug_timestamp);
 if (m_debug_timestamp) {
        time_stamp_dts.set_timestamp_reorder_mode(true);
        time_stamp_dts.enable_debug_print(true);
 }

    property_value[0] = '\0';
    property_get("vidc.dec.debug.concealedmb", property_value, "0");
    m_debug_concealedmb = atoi(property_value);
    DEBUG_PRINT_HIGH("vidc.dec.debug.concealedmb value is %d",m_debug_concealedmb);

    property_value[0] = '\0';
    property_get("vidc.dec.profile.check", property_value, "0");
    m_reject_avc_1080p_mp = atoi(property_value);
    DEBUG_PRINT_HIGH("vidc.dec.profile.check value is %d",m_reject_avc_1080p_mp);

    property_value[0] = '\0';
    property_get("vidc.dec.log.in", property_value, "0");
    m_debug.in_buffer_log = atoi(property_value);

    property_value[0] = '\0';
    property_get("vidc.dec.log.out", property_value, "0");
    m_debug.out_buffer_log = atoi(property_value);
    sprintf(m_debug.log_loc, "%s", BUFFER_LOG_LOC);

    property_value[0] = '\0';
    property_get("vidc.log.loc", property_value, "");
 if (*property_value)
        strlcpy(m_debug.log_loc, property_value, PROPERTY_VALUE_MAX);

    property_value[0] = '\0';
    property_get("vidc.dec.120fps.enabled", property_value, "0");

 if(atoi(property_value)) {
        DEBUG_PRINT_LOW("feature 120 FPS decode enabled");
        m_last_rendered_TS = 0;
 }

    property_value[0] = '\0';
    property_get("vidc.dec.debug.dyn.disabled", property_value, "0");
    m_disable_dynamic_buf_mode = atoi(property_value);
    DEBUG_PRINT_HIGH("vidc.dec.debug.dyn.disabled value is %d",m_disable_dynamic_buf_mode);

#endif
    memset(&m_cmp,0,sizeof(m_cmp));
    memset(&m_cb,0,sizeof(m_cb));
    memset (&drv_ctx,0,sizeof(drv_ctx));
    memset (&h264_scratch,0,sizeof (OMX_BUFFERHEADERTYPE));
    memset (m_hwdevice_name,0,sizeof(m_hwdevice_name));
    memset(m_demux_offsets, 0, ( sizeof(OMX_U32) * 8192) );
    memset(&m_custom_buffersize, 0, sizeof(m_custom_buffersize));
    m_demux_entries = 0;
    msg_thread_id = 0;
    async_thread_id = 0;
    msg_thread_created = false;
    async_thread_created = false;
#ifdef _ANDROID_ICS_
    memset(&native_buffer, 0 ,(sizeof(struct nativebuffer) * MAX_NUM_INPUT_OUTPUT_BUFFERS));
#endif
    memset(&drv_ctx.extradata_info, 0, sizeof(drv_ctx.extradata_info));

 /* invalidate m_frame_pack_arrangement */
    memset(&m_frame_pack_arrangement, 0, sizeof(OMX_QCOM_FRAME_PACK_ARRANGEMENT));
    m_frame_pack_arrangement.cancel_flag = 1;

    drv_ctx.timestamp_adjust = false;
    drv_ctx.video_driver_fd = -1;

     m_vendor_config.pData = NULL;
     pthread_mutex_init(&m_lock, NULL);
     pthread_mutex_init(&c_lock, NULL);
     sem_init(&m_cmd_lock,0,0);
     sem_init(&m_safe_flush, 0, 0);
     streaming[CAPTURE_PORT] =
        streaming[OUTPUT_PORT] = false;
#ifdef _ANDROID_
 char extradata_value[PROPERTY_VALUE_MAX] = {0};
    property_get("vidc.dec.debug.extradata", extradata_value, "0");
    m_debug_extradata = atoi(extradata_value);
    DEBUG_PRINT_HIGH("vidc.dec.debug.extradata value is %d",m_debug_extradata);
#endif
    m_fill_output_msg = OMX_COMPONENT_GENERATE_FTB;
    client_buffers.set_vdec_client(this);
    dynamic_buf_mode = false;
    out_dynamic_list = NULL;
    is_down_scalar_enabled = false;
    m_smoothstreaming_mode = false;
    m_smoothstreaming_width = 0;
    m_smoothstreaming_height = 0;
    is_q6_platform = false;
}
