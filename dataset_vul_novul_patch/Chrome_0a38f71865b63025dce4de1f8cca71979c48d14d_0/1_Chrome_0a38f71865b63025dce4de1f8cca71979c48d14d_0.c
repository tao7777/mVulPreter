void BluetoothOptionsHandler::DeviceNotification(
    const DictionaryValue& device) {
   web_ui_->CallJavascriptFunction(
      "options.SystemOptions.addBluetoothDevice", device);
 }
