 void BluetoothOptionsHandler::RequestPasskey(
     chromeos::BluetoothDevice* device) {
  DictionaryValue params;
  params.SetString("pairing", "bluetoothEnterPasskey");
  SendDeviceNotification(device, &params);
 }
