 void PromoResourceService::PostNotification(int64 delay_ms) {
  // Note that this could cause re-issuing a notification every time
  // we receive an update from a server if something goes wrong.
  // Given that this couldn't happen more frequently than every
  // kCacheUpdateDelay milliseconds, we should be fine.
   if (delay_ms > 0) {
     MessageLoop::current()->PostDelayedTask(
         FROM_HERE,
         base::Bind(&PromoResourceService::PromoResourceStateChange,
                   weak_ptr_factory_.GetWeakPtr()),
        base::TimeDelta::FromMilliseconds(delay_ms));
  } else if (delay_ms == 0) {
    PromoResourceStateChange();
  }
 }
