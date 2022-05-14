void QuotaManager::GetAvailableSpace(const AvailableSpaceCallback& callback) {
  if (is_incognito_) {
     callback.Run(kQuotaStatusOk, kIncognitoDefaultTemporaryQuota);
     return;
   }
  make_scoped_refptr(new AvailableSpaceQueryTask(this, callback))->Start();
 }
