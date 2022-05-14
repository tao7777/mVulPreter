static void timer_enter_running(Timer *t) {
        _cleanup_bus_error_free_ sd_bus_error error = SD_BUS_ERROR_NULL;
        int r;

        assert(t);

        /* Don't start job if we are supposed to go down */
        if (unit_stop_pending(UNIT(t)))
                return;

        r = manager_add_job(UNIT(t)->manager, JOB_START, UNIT_TRIGGER(UNIT(t)),
                            JOB_REPLACE, true, &error, NULL);
        if (r < 0)
                goto fail;

         dual_timestamp_get(&t->last_trigger);
 
         if (t->stamp_path)
                touch_file(t->stamp_path, true, t->last_trigger.realtime, UID_INVALID, GID_INVALID, 0);
 
         timer_set_state(t, TIMER_RUNNING);
         return;

fail:
        log_unit_warning(UNIT(t), "Failed to queue unit startup job: %s", bus_error_message(&error, r));
        timer_enter_dead(t, TIMER_FAILURE_RESOURCES);
}
