 void BluetoothDeviceChooserController::PostErrorCallback(
    WebBluetoothResult error) {
   if (!base::ThreadTaskRunnerHandle::Get()->PostTask(
           FROM_HERE, base::BindOnce(error_callback_, error))) {
     LOG(WARNING) << "No TaskRunner.";
  }
}
