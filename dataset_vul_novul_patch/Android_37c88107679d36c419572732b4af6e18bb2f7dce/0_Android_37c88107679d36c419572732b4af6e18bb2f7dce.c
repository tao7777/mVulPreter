static int enable(void) {
static int enable(bool start_restricted) {
  LOG_INFO(LOG_TAG, "%s: start restricted = %d", __func__, start_restricted);

  restricted_mode = start_restricted;
 
   if (!interface_ready())
     return BT_STATUS_NOT_READY;

  stack_manager_get_interface()->start_up_stack_async();
 return BT_STATUS_SUCCESS;
}
