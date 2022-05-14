int main(int argc, char **argv) {
 if (!parse_args(argc, argv)) {
    usage(argv[0]);
 }

 if (bond && discoverable) {
    fprintf(stderr, "Can only select either bond or discoverable, not both\n");
    usage(argv[0]);
 }

 if (sco_listen && sco_connect) {
    fprintf(stderr, "Can only select either sco_listen or sco_connect, not both\n");
    usage(argv[0]);
 }

 if (!bond && !discover && !discoverable && !up && !get_name && !set_name && !sco_listen && !sco_connect) {
    fprintf(stderr, "Must specify one command\n");
    usage(argv[0]);
 }

 if (signal(SIGINT, sig_handler) == SIG_ERR) {
    fprintf(stderr, "Will be unable to catch signals\n");
 }

  fprintf(stdout, "Bringing up bluetooth adapter\n");
 if (!hal_open(callbacks_get_adapter_struct())) {
    fprintf(stderr, "Unable to open Bluetooth HAL.\n");
 return 1;

   }
 
   if (discover) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     fprintf(stdout, "Starting to start discovery\n");
    CALL_AND_WAIT(bt_interface->start_discovery(), discovery_state_changed);
    fprintf(stdout, "Started discovery for %d seconds\n", timeout_in_sec);

    sleep(timeout_in_sec);

    fprintf(stdout, "Starting to cancel discovery\n");
    CALL_AND_WAIT(bt_interface->cancel_discovery(), discovery_state_changed);
    fprintf(stdout, "Cancelled discovery after %d seconds\n", timeout_in_sec);

   }
 
   if (discoverable) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     bt_property_t *property = property_new_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

 int rc = bt_interface->set_adapter_property(property);
    fprintf(stdout, "Set rc:%d device as discoverable for %d seconds\n", rc, timeout_in_sec);

    sleep(timeout_in_sec);

    property_free(property);
 }

 if (bond) {
 if (bdaddr_is_empty(&bt_remote_bdaddr)) {
      fprintf(stderr, "Must specify a remote device address [ --bdaddr=xx:yy:zz:aa:bb:cc ]\n");

       exit(1);
     }
 
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     int rc = bt_interface->create_bond(&bt_remote_bdaddr, 0 /* UNKNOWN; Currently not documented :( */);
    fprintf(stdout, "Started bonding:%d for %d seconds\n", rc, timeout_in_sec);

    sleep(timeout_in_sec);

   }
 
   if (up) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     fprintf(stdout, "Waiting for %d seconds\n", timeout_in_sec);
    sleep(timeout_in_sec);

   }
 
   if (get_name) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
     int error;
     CALL_AND_WAIT(error = bt_interface->get_adapter_property(BT_PROPERTY_BDNAME), adapter_properties);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to get adapter property\n");
      exit(1);
 }
 bt_property_t *property = adapter_get_property(BT_PROPERTY_BDNAME);
 const bt_bdname_t *name = property_as_name(property);
 if (name)
      printf("Queried bluetooth device name:%s\n", name->name);
 else
      printf("No name\n");

   }
 
   if (set_name) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     bt_property_t *property = property_new_name(bd_name);
    printf("Setting bluetooth device name to:%s\n", bd_name);
 int error;
    CALL_AND_WAIT(error = bt_interface->set_adapter_property(property), adapter_properties);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to set adapter property\n");
      exit(1);
 }
    CALL_AND_WAIT(error = bt_interface->get_adapter_property(BT_PROPERTY_BDNAME), adapter_properties);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to get adapter property\n");
      exit(1);
 }
    property_free(property);
    sleep(timeout_in_sec);

   }
 
   if (sco_listen) {
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     bt_property_t *property = property_new_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
    CALL_AND_WAIT(bt_interface->set_adapter_property(property), adapter_properties);
    property_free(property);

 const btsock_interface_t *sock = bt_interface->get_profile_interface(BT_PROFILE_SOCKETS_ID);

 int rfcomm_fd = INVALID_FD;
 int error = sock->listen(BTSOCK_RFCOMM, "meow", (const uint8_t *)&HFP_AG_UUID, 0, &rfcomm_fd, 0);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to listen for incoming RFCOMM socket: %d\n", error);
      exit(1);
 }

 int sock_fd = INVALID_FD;
    error = sock->listen(BTSOCK_SCO, NULL, NULL, 5, &sock_fd, 0);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to listen for incoming SCO sockets: %d\n", error);
      exit(1);
 }
    fprintf(stdout, "Waiting for incoming SCO connections...\n");
    sleep(timeout_in_sec);
 }

 if (sco_connect) {
 if (bdaddr_is_empty(&bt_remote_bdaddr)) {
      fprintf(stderr, "Must specify a remote device address [ --bdaddr=xx:yy:zz:aa:bb:cc ]\n");

       exit(1);
     }
 
    CALL_AND_WAIT(bt_interface->enable(), adapter_state_changed);
     fprintf(stdout, "BT adapter is up\n");
 
     const btsock_interface_t *sock = bt_interface->get_profile_interface(BT_PROFILE_SOCKETS_ID);

 int rfcomm_fd = INVALID_FD;
 int error = sock->connect(&bt_remote_bdaddr, BTSOCK_RFCOMM, (const uint8_t *)&HFP_AG_UUID, 0, &rfcomm_fd, 0);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to connect to RFCOMM socket: %d.\n", error);
      exit(1);
 }

    WAIT(acl_state_changed);

    fprintf(stdout, "Establishing SCO connection...\n");

 int sock_fd = INVALID_FD;
    error = sock->connect(&bt_remote_bdaddr, BTSOCK_SCO, NULL, 5, &sock_fd, 0);
 if (error != BT_STATUS_SUCCESS) {
      fprintf(stderr, "Unable to connect to SCO socket: %d.\n", error);
      exit(1);
 }
    sleep(timeout_in_sec);
 }

  CALL_AND_WAIT(bt_interface->disable(), adapter_state_changed);
  fprintf(stdout, "BT adapter is down\n");
}
