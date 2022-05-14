void QuotaManager::GetAvailableSpace(const AvailableSpaceCallback& callback) {
  if (is_incognito_) {
     callback.Run(kQuotaStatusOk, kIncognitoDefaultTemporaryQuota);
     return;
   }

  PostTaskAndReplyWithResult(
      db_thread_,
      FROM_HERE,
      base::Bind(get_disk_space_fn_, profile_path_),
      base::Bind(&QuotaManager::DidGetAvailableSpace,
                 weak_factory_.GetWeakPtr(),
                 callback));
 }
