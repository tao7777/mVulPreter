 void BluetoothOptionsHandler::GenerateFakeDeviceList() {
  GenerateFakeDevice(

     "Fake Wireless Keyboard",
     "01-02-03-04-05-06",
     "input-keyboard",
     true,
    true,
    "");
  GenerateFakeDevice(
     "Fake Wireless Mouse",
     "02-03-04-05-06-01",
     "input-mouse",
     true,
    false,
    "");
  GenerateFakeDevice(
     "Fake Wireless Headset",
     "03-04-05-06-01-02",
     "headset",
     false,
    false,
    "");
  GenerateFakeDevice(
     "Fake Connecting Keyboard",
     "04-05-06-01-02-03",
     "input-keyboard",
    false,
    false,
     "bluetoothRemotePasskey");
  GenerateFakeDevice(
     "Fake Connecting Phone",
     "05-06-01-02-03-04",
     "phone",
    false,
    false,
     "bluetoothConfirmPasskey");
  GenerateFakeDevice(
     "Fake Connecting Headset",
     "06-01-02-03-04-05",
     "headset",
    false,
    false,
     "bluetoothEnterPasskey");
 
   web_ui_->CallJavascriptFunction(
       "options.SystemOptions.notifyBluetoothSearchComplete");
 }
