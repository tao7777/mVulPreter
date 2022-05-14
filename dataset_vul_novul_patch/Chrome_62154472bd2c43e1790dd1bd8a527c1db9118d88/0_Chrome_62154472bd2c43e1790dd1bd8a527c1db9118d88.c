void BluetoothDeviceChooserController::GetDevice(
    blink::mojom::WebBluetoothRequestDeviceOptionsPtr options,
    const SuccessCallback& success_callback,
    const ErrorCallback& error_callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  DCHECK(success_callback_.is_null());
  DCHECK(error_callback_.is_null());

  success_callback_ = success_callback;
  error_callback_ = error_callback;
  options_ = std::move(options);
  LogRequestDeviceOptions(options_);

  if (options_->filters &&
       BluetoothBlocklist::Get().IsExcluded(options_->filters.value())) {
     RecordRequestDeviceOutcome(
         UMARequestDeviceOutcome::BLOCKLISTED_SERVICE_IN_FILTER);
    PostErrorCallback(WebBluetoothResult::REQUEST_DEVICE_WITH_BLOCKLISTED_UUID);
     return;
   }
   BluetoothBlocklist::Get().RemoveExcludedUUIDs(options_.get());
 
  WebBluetoothResult allow_result =
      web_bluetooth_service_->GetBluetoothAllowed();
  if (allow_result != WebBluetoothResult::SUCCESS) {
    switch (allow_result) {
      case WebBluetoothResult::CHOOSER_NOT_SHOWN_API_LOCALLY_DISABLED: {
        RecordRequestDeviceOutcome(
            UMARequestDeviceOutcome::BLUETOOTH_CHOOSER_POLICY_DISABLED);
        break;
      }
      case WebBluetoothResult::CHOOSER_NOT_SHOWN_API_GLOBALLY_DISABLED: {
        // Log to the developer console.
        web_contents_->GetMainFrame()->AddMessageToConsole(
            blink::mojom::ConsoleMessageLevel::kInfo,
            "Bluetooth permission has been blocked.");
        // Block requests.
        RecordRequestDeviceOutcome(
            UMARequestDeviceOutcome::BLUETOOTH_GLOBALLY_DISABLED);
        break;
      }
      default:
        break;
    }
    PostErrorCallback(allow_result);
     return;
   }
 
   if (!adapter_->IsPresent()) {
     DVLOG(1) << "Bluetooth Adapter not present. Can't serve requestDevice.";
     RecordRequestDeviceOutcome(
         UMARequestDeviceOutcome::BLUETOOTH_ADAPTER_NOT_PRESENT);
    PostErrorCallback(WebBluetoothResult::NO_BLUETOOTH_ADAPTER);
     return;
   }
 
   BluetoothChooser::EventHandler chooser_event_handler =
       base::Bind(&BluetoothDeviceChooserController::OnBluetoothChooserEvent,
                  base::Unretained(this));

  if (WebContentsDelegate* delegate = web_contents_->GetDelegate()) {
    chooser_ = delegate->RunBluetoothChooser(render_frame_host_,
                                             std::move(chooser_event_handler));
   }
 
   if (!chooser_.get()) {
    PostErrorCallback(WebBluetoothResult::WEB_BLUETOOTH_NOT_SUPPORTED);
     return;
   }
 
  if (!chooser_->CanAskForScanningPermission()) {
    DVLOG(1) << "Closing immediately because Chooser cannot obtain permission.";
    OnBluetoothChooserEvent(BluetoothChooser::Event::DENIED_PERMISSION,
                            "" /* device_address */);
    return;
  }

  device_ids_.clear();
  PopulateConnectedDevices();
  if (!chooser_.get()) {
    return;
  }

  if (!adapter_->IsPowered()) {
    chooser_->SetAdapterPresence(
        BluetoothChooser::AdapterPresence::POWERED_OFF);
    return;
  }

  StartDeviceDiscovery();
}
