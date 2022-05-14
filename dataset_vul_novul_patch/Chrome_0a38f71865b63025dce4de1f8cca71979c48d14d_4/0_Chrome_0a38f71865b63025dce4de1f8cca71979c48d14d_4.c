void BluetoothOptionsHandler::GenerateFakePairing(
    bool connected,
     const std::string& pairing) {
  DictionaryValue properties;
  properties.SetString(bluetooth_device::kNameProperty, name);
  properties.SetString(bluetooth_device::kAddressProperty, address);
  properties.SetString(bluetooth_device::kIconProperty, icon);
  properties.SetBoolean(bluetooth_device::kPairedProperty, paired);
  properties.SetBoolean(bluetooth_device::kConnectedProperty, connected);
  properties.SetInteger(bluetooth_device::kClassProperty, 0);
  chromeos::BluetoothDevice* device =
      chromeos::BluetoothDevice::Create(properties);
  DeviceFound("FakeAdapter", device);
  if (pairing.compare("bluetoothRemotePasskey") == 0) {
    DisplayPasskey(device, 12345, 2);
  } else if (pairing.compare("bluetoothConfirmPasskey") == 0) {
    RequestConfirmation(device, 12345);
  } else if (pairing.compare("bluetoothEnterPasskey") == 0) {
    RequestPasskey(device);
  }
  delete device;
 }
