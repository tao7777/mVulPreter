void PromoResourceService::ScheduleNotification(double promo_start,
                                                double promo_end) {
  if (promo_start > 0 && promo_end > 0) {
    const int64 ms_until_start =
        static_cast<int64>((base::Time::FromDoubleT(
            promo_start) - base::Time::Now()).InMilliseconds());
    const int64 ms_until_end =
        static_cast<int64>((base::Time::FromDoubleT(
            promo_end) - base::Time::Now()).InMilliseconds());
    if (ms_until_start > 0) {
       PostNotification(ms_until_start);
     } else if (ms_until_end > 0) {
       if (ms_until_start <= 0) {
        // The promo is active.  Notify immediately.
         PostNotification(0);
       }
       PostNotification(ms_until_end);
    } else {
      // The promo (if any) has finished.  Notify immediately.
      PostNotification(0);
     }
  } else {
      // The promo (if any) was apparently cancelled.  Notify immediately.
      PostNotification(0);
   }
 }
