void BinaryUploadService::IsAuthorized(AuthorizationCallback callback) {
  if (!timer_.IsRunning()) {
    timer_.Start(FROM_HERE, base::TimeDelta::FromHours(24), this,
                 &BinaryUploadService::ResetAuthorizationData);
  }

   if (!can_upload_data_.has_value()) {
     if (!pending_validate_data_upload_request_) {
      std::string dm_token = GetDMToken();
      if (dm_token.empty()) {
         std::move(callback).Run(false);
         return;
       }

      pending_validate_data_upload_request_ = true;
       auto request = std::make_unique<ValidateDataUploadRequest>(base::BindOnce(
           &BinaryUploadService::ValidateDataUploadRequestCallback,
           weakptr_factory_.GetWeakPtr()));
      request->set_dm_token(dm_token);
       UploadForDeepScanning(std::move(request));
     }
     authorization_callbacks_.push_back(std::move(callback));
    return;
  }
  std::move(callback).Run(can_upload_data_.value());
}
