static int enable(void) {
  LOG_INFO("%s", __func__);
 
   if (!interface_ready())
     return BT_STATUS_NOT_READY;

  stack_manager_get_interface()->start_up_stack_async();
 return BT_STATUS_SUCCESS;
}
