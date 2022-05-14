PushMessagingServiceImpl::PushMessagingServiceImpl(Profile* profile)
    : profile_(profile),
       push_subscription_count_(0),
       pending_push_subscription_count_(0),
       notification_manager_(profile),
      push_messaging_service_observer_(PushMessagingServiceObserver::Create()),
       weak_factory_(this) {
   DCHECK(profile);
   HostContentSettingsMapFactory::GetForProfile(profile_)->AddObserver(this);

  registrar_.Add(this, chrome::NOTIFICATION_APP_TERMINATING,
                 content::NotificationService::AllSources());
}
