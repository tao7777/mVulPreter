void NotificationService::RemoveObserver(NotificationObserver* observer,
                                          NotificationType type,
                                          const NotificationSource& source) {
   DCHECK(type.value < NotificationType::NOTIFICATION_TYPE_COUNT);
  DCHECK(HasKey(observers_[type.value], source));
 
   NotificationObserverList* observer_list =
       observers_[type.value][source.map_key()];
  if (observer_list) {
    observer_list->RemoveObserver(observer);
#ifndef NDEBUG
    --observer_counts_[type.value];
#endif
  }

}
