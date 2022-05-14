 void PromoResourceService::PromoResourceStateChange() {
  web_resource_update_scheduled_ = false;
   content::NotificationService* service =
       content::NotificationService::current();
   service->Notify(chrome::NOTIFICATION_PROMO_RESOURCE_STATE_CHANGED,
                  content::Source<WebResourceService>(this),
                  content::NotificationService::NoDetails());
}
