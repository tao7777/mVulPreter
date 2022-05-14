 void WebBluetoothServiceImpl::ClearState() {
  // Releasing the adapter will drop references to callbacks that have not yet
  // been executed. The binding must be closed first so that this is allowed.
  binding_.Close();

   characteristic_id_to_notify_session_.clear();
   pending_primary_services_requests_.clear();
   descriptor_id_to_characteristic_id_.clear();
  characteristic_id_to_service_id_.clear();
  service_id_to_device_address_.clear();
  connected_devices_.reset(
      new FrameConnectedBluetoothDevices(render_frame_host_));
  device_chooser_controller_.reset();
  BluetoothAdapterFactoryWrapper::Get().ReleaseAdapter(this);
}
