 void BluetoothOptionsHandler::DisplayPasskey(
     chromeos::BluetoothDevice* device,
     int passkey,
     int entered) {
  DictionaryValue params;
  params.SetString("pairing", "bluetoothRemotePasskey");
  params.SetInteger("passkey", passkey);
  params.SetInteger("entered", entered);
  SendDeviceNotification(device, &params);
 }
