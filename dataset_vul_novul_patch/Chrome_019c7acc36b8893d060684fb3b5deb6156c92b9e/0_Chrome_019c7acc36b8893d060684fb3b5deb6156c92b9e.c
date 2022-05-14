void NotificationService::RemoveObserver(NotificationObserver* observer,
                                          NotificationType type,
                                          const NotificationSource& source) {
   DCHECK(type.value < NotificationType::NOTIFICATION_TYPE_COUNT);

  // This is a very serious bug.  An object is most likely being deleted on
  // the wrong thread, and as a result another thread's NotificationService
  // has its deleted pointer in its map.  A garbge object will be called in the
  // future.
  // NOTE: when this check shows crashes, use ChromeThread::DeleteOnIOThread or
  // other variants as the trait on the object.
  CHECK(HasKey(observers_[type.value], source));
 
   NotificationObserverList* observer_list =
       observers_[type.value][source.map_key()];
  if (observer_list) {
    observer_list->RemoveObserver(observer);
#ifndef NDEBUG
    --observer_counts_[type.value];
#endif
  }

}
