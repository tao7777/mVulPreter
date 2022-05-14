void BluetoothOptionsHandler::GenerateFakePairing(
    const std::string& name,
    const std::string& address,
    const std::string& icon,
     const std::string& pairing) {
  DictionaryValue device;
  device.SetString("name", name);
  device.SetString("address", address);
  device.SetString("icon", icon);
  device.SetBoolean("paired", false);
  device.SetBoolean("connected", false);
  DictionaryValue op;
  op.SetString("pairing", pairing);
  if (pairing.compare("bluetoothEnterPasskey") != 0)
    op.SetInteger("passkey", 12345);
  if (pairing.compare("bluetoothRemotePasskey") == 0)
    op.SetInteger("entered", 2);
  web_ui_->CallJavascriptFunction(
      "options.SystemOptions.connectBluetoothDevice", device, op);
 }
