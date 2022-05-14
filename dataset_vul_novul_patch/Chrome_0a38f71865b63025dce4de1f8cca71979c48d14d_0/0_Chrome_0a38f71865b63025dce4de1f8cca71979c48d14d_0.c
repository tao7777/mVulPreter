void BluetoothOptionsHandler::DeviceNotification(
void BluetoothOptionsHandler::SendDeviceNotification(
    chromeos::BluetoothDevice* device,
    base::DictionaryValue* params) {
  // Retrieve properties of the bluetooth device.  The properties names are
  // in title case.  Convert to camel case in accordance with our Javascript
  // naming convention.
  const DictionaryValue& properties = device->AsDictionary();
  base::DictionaryValue js_properties;
  for (DictionaryValue::key_iterator it = properties.begin_keys();
      it != properties.end_keys(); ++it) {
    base::Value* child = NULL;
    properties.GetWithoutPathExpansion(*it, &child);
    if (child) {
      std::string js_key = *it;
      js_key[0] = tolower(js_key[0]);
      js_properties.SetWithoutPathExpansion(js_key, child->DeepCopy());
    }
  }
  if (params) {
    js_properties.MergeDictionary(params);
  }
   web_ui_->CallJavascriptFunction(
      "options.SystemOptions.addBluetoothDevice",
      js_properties);
 }
