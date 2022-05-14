void BluetoothOptionsHandler::GenerateFakeDiscoveredDevice(
     const std::string& name,
     const std::string& address,
     const std::string& icon,
     bool paired,
    bool connected) {
  DictionaryValue device;
  device.SetString("name", name);
  device.SetString("address", address);
  device.SetString("icon", icon);
  device.SetBoolean("paired", paired);
  device.SetBoolean("connected", connected);
  web_ui_->CallJavascriptFunction(
      "options.SystemOptions.addBluetoothDevice", device);
}
