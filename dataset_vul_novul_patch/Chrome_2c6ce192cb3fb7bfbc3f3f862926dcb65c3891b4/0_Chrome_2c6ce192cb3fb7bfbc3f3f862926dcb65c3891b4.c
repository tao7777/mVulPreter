BluetoothChooserDesktop::~BluetoothChooserDesktop() {
   bluetooth_chooser_controller_->ResetEventHandler();
  if (bubble_)
    bubble_->CloseBubble(BUBBLE_CLOSE_FORCED);
 }
