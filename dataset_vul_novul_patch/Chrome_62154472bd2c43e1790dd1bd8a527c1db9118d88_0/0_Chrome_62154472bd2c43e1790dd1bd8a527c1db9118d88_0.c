void BluetoothDeviceChooserController::OnBluetoothChooserEvent(
    BluetoothChooser::Event event,
    const std::string& device_address) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  DCHECK(chooser_.get());

  switch (event) {
    case BluetoothChooser::Event::RESCAN:
      RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      device_ids_.clear();
      PopulateConnectedDevices();
      DCHECK(chooser_);
      StartDeviceDiscovery();
       return;
     case BluetoothChooser::Event::DENIED_PERMISSION:
       RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      PostErrorCallback(
          WebBluetoothResult::CHOOSER_NOT_SHOWN_USER_DENIED_PERMISSION_TO_SCAN);
       break;
     case BluetoothChooser::Event::CANCELLED:
       RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      PostErrorCallback(WebBluetoothResult::CHOOSER_CANCELLED);
       break;
     case BluetoothChooser::Event::SHOW_OVERVIEW_HELP:
       DVLOG(1) << "Overview Help link pressed.";
       RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      PostErrorCallback(WebBluetoothResult::CHOOSER_CANCELLED);
       break;
     case BluetoothChooser::Event::SHOW_ADAPTER_OFF_HELP:
       DVLOG(1) << "Adapter Off Help link pressed.";
       RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      PostErrorCallback(WebBluetoothResult::CHOOSER_CANCELLED);
       break;
     case BluetoothChooser::Event::SHOW_NEED_LOCATION_HELP:
       DVLOG(1) << "Need Location Help link pressed.";
       RecordRequestDeviceOutcome(OutcomeFromChooserEvent(event));
      PostErrorCallback(WebBluetoothResult::CHOOSER_CANCELLED);
       break;
     case BluetoothChooser::Event::SELECTED:
       RecordNumOfDevices(options_->accept_all_devices, device_ids_.size());
      PostSuccessCallback(device_address);
      break;
  }
  chooser_.reset();
}
