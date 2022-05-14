void AudioOutputAuthorizationHandler::GetDeviceParameters(
    AuthorizationCompletedCallback cb,
     const std::string& raw_device_id) const {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
   DCHECK(!raw_device_id.empty());
  base::PostTaskAndReplyWithResult(
      audio_manager_->GetTaskRunner(), FROM_HERE,
      base::Bind(&GetDeviceParametersOnDeviceThread,
                 base::Unretained(audio_manager_), raw_device_id),
       base::Bind(&AudioOutputAuthorizationHandler::DeviceParametersReceived,
                  weak_factory_.GetWeakPtr(), std::move(cb), false,
                  raw_device_id));
}
