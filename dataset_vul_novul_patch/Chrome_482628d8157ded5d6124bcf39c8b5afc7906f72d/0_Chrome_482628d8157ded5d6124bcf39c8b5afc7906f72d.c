void setSharedTimerFireTime(double fireTime)
{
    ASSERT(sharedTimerFiredFunction);

    double interval = fireTime - currentTime();
    guint intervalInMS;
    if (interval < 0)
        intervalInMS = 0;
    else {
        interval *= 1000;
        intervalInMS = (guint)interval;
    }
 
     stopSharedTimer();
     if (intervalInMS == 0)
        sharedTimer = g_idle_add(timeout_cb, NULL);
     else
         sharedTimer = g_timeout_add_full(G_PRIORITY_DEFAULT, intervalInMS, timeout_cb, NULL, NULL);
 }
