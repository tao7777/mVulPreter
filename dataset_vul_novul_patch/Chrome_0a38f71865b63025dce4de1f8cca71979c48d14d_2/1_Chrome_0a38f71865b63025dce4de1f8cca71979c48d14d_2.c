 void BluetoothOptionsHandler::GenerateFakeDeviceList() {
  GenerateFakeDiscoveredDevice(
     "Fake Wireless Keyboard",
     "01-02-03-04-05-06",
     "input-keyboard",
     true,
    true);
  GenerateFakeDiscoveredDevice(
     "Fake Wireless Mouse",
     "02-03-04-05-06-01",
     "input-mouse",
     true,
    false);
  GenerateFakeDiscoveredDevice(
     "Fake Wireless Headset",
     "03-04-05-06-01-02",
     "headset",
     false,
    false);
  GenerateFakePairing(
     "Fake Connecting Keyboard",
     "04-05-06-01-02-03",
     "input-keyboard",
     "bluetoothRemotePasskey");
  GenerateFakePairing(
     "Fake Connecting Phone",
     "05-06-01-02-03-04",
     "phone",
     "bluetoothConfirmPasskey");
  GenerateFakePairing(
     "Fake Connecting Headset",
     "06-01-02-03-04-05",
     "headset",
     "bluetoothEnterPasskey");
 
   web_ui_->CallJavascriptFunction(
       "options.SystemOptions.notifyBluetoothSearchComplete");
 }
