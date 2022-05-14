static void command_timed_out(UNUSED_ATTR void *context) {
  pthread_mutex_lock(&commands_pending_response_lock);

 if (list_is_empty(commands_pending_response)) {
    LOG_ERROR("%s with no commands pending response", __func__);
 } else {
 waiting_command_t *wait_entry = list_front(commands_pending_response);
    pthread_mutex_unlock(&commands_pending_response_lock);

    LOG_ERROR("%s hci layer timeout waiting for response to a command. opcode: 0x%x", __func__, wait_entry->opcode);

   }
 
   LOG_ERROR("%s restarting the bluetooth process.", __func__);
  TEMP_FAILURE_RETRY(usleep(10000));
   kill(getpid(), SIGKILL);
 }
