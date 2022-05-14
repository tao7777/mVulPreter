static int vol_prc_lib_release(effect_handle_t handle)
{

     struct listnode *node, *temp_node_next;
     vol_listener_context_t *context = NULL;
     vol_listener_context_t *recv_contex = (vol_listener_context_t *)handle;
    int status = -1;
     bool recompute_flag = false;
     int active_stream_count = 0;
     ALOGV("%s context %p", __func__, handle);
     pthread_mutex_lock(&vol_listner_init_lock);
 
     list_for_each_safe(node, temp_node_next, &vol_effect_list) {
         context = node_to_item(node, struct vol_listener_context_s, effect_list_node);
        if ((memcmp(&(context->desc->uuid), &(recv_contex->desc->uuid), sizeof(effect_uuid_t)) == 0)
            && (context->session_id == recv_contex->session_id)
            && (context->stream_type == recv_contex->stream_type)) {
             ALOGV("--- Found something to remove ---");
            list_remove(&context->effect_list_node);
             PRINT_STREAM_TYPE(context->stream_type);
             if (context->dev_id && AUDIO_DEVICE_OUT_SPEAKER) {
                 recompute_flag = true;
 }
            free(context);
            status = 0;
 } else {
 ++active_stream_count;
 }
 }

 
     if (status != 0) {
         ALOGE("something wrong ... <<<--- Found NOTHING to remove ... ???? --->>>>>");
     }
 
 if (active_stream_count == 0) {
        current_gain_dep_cal_level = -1;
        current_vol = 0.0;
 }

 if (recompute_flag) {
        check_and_set_gain_dep_cal();
 }

 if (dumping_enabled) {
        dump_list_l();
 }
    pthread_mutex_unlock(&vol_listner_init_lock);
 return status;
}
