void BluetoothOptionsHandler::DeviceFound(const std::string& adapter_id,
                                           chromeos::BluetoothDevice* device) {
   VLOG(2) << "Device found on " << adapter_id;
   DCHECK(device);
  web_ui_->CallJavascriptFunction(
      "options.SystemOptions.addBluetoothDevice", device->AsDictionary());
 }
