PromoResourceService::PromoResourceService(Profile* profile)
    : WebResourceService(profile->GetPrefs(),
                         GetPromoResourceURL(),
                         true,  // append locale to URL
                         prefs::kNtpPromoResourceCacheUpdate,
                         kStartResourceFetchDelay,
                          GetCacheUpdateDelay()),
                          profile_(profile),
                          ALLOW_THIS_IN_INITIALIZER_LIST(
                             weak_ptr_factory_(this)) {
   ScheduleNotificationOnInit();
 }
