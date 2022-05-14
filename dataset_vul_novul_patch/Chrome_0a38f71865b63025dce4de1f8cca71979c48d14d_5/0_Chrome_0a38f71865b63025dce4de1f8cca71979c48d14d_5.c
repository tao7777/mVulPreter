 void BluetoothOptionsHandler::RequestConfirmation(
     chromeos::BluetoothDevice* device,
     int passkey) {
  DictionaryValue params;
  params.SetString("pairing", "bluetoothConfirmPasskey");
  params.SetInteger("passkey", passkey);
  SendDeviceNotification(device, &params);
 }
