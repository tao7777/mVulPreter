 void PromoResourceService::PostNotification(int64 delay_ms) {
  if (web_resource_update_scheduled_)
    return;
   if (delay_ms > 0) {
    web_resource_update_scheduled_ = true;
     MessageLoop::current()->PostDelayedTask(
         FROM_HERE,
         base::Bind(&PromoResourceService::PromoResourceStateChange,
                   weak_ptr_factory_.GetWeakPtr()),
        base::TimeDelta::FromMilliseconds(delay_ms));
  } else if (delay_ms == 0) {
    PromoResourceStateChange();
  }
 }
