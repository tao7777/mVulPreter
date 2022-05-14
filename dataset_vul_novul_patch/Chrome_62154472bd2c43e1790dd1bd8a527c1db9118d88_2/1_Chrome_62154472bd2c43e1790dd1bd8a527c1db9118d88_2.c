BluetoothDeviceChooserController::~BluetoothDeviceChooserController() {
  if (scanning_start_time_) {
    RecordScanningDuration(base::TimeTicks::Now() -
                           scanning_start_time_.value());
  }
 
   if (chooser_) {
     DCHECK(!error_callback_.is_null());
    error_callback_.Run(blink::mojom::WebBluetoothResult::CHOOSER_CANCELLED);
   }
 }
