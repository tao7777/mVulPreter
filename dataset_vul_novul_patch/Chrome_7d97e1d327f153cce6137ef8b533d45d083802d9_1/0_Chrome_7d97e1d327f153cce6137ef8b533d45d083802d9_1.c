 void PromoResourceService::PromoResourceStateChange() {
   content::NotificationService* service =
       content::NotificationService::current();
   service->Notify(chrome::NOTIFICATION_PROMO_RESOURCE_STATE_CHANGED,
                  content::Source<WebResourceService>(this),
                  content::NotificationService::NoDetails());
}
