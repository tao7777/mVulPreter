 void UserActivityDetector::MaybeNotify() {
  base::TimeTicks now = base::TimeTicks::Now();
   if (last_observer_notification_time_.is_null() ||
       (now - last_observer_notification_time_).InSecondsF() >=
       kNotifyIntervalSec) {
    FOR_EACH_OBSERVER(UserActivityObserver, observers_, OnUserActivity());
    last_observer_notification_time_ = now;
  }
}
