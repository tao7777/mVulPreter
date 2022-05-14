 bool adapter_enable_disable() {
   int error;
 
  CALL_AND_WAIT(error = bt_interface->enable(false), adapter_state_changed);
   TASSERT(error == BT_STATUS_SUCCESS, "Error enabling Bluetooth: %d", error);
   TASSERT(adapter_get_state() == BT_STATE_ON, "Adapter did not turn on.");
 
  CALL_AND_WAIT(error = bt_interface->disable(), adapter_state_changed);
  TASSERT(error == BT_STATUS_SUCCESS, "Error disabling Bluetooth: %d", error);
  TASSERT(adapter_get_state() == BT_STATE_OFF, "Adapter did not turn off.");

 return true;
}
