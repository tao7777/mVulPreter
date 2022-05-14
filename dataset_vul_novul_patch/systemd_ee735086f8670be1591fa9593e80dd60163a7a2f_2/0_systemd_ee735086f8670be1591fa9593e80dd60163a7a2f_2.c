static int timer_start(Unit *u) {
        Timer *t = TIMER(u);
        TimerValue *v;

        assert(t);
        assert(t->state == TIMER_DEAD || t->state == TIMER_FAILED);

        if (UNIT_TRIGGER(u)->load_state != UNIT_LOADED)
                return -ENOENT;

        t->last_trigger = DUAL_TIMESTAMP_NULL;

        /* Reenable all timers that depend on unit activation time */
        LIST_FOREACH(value, v, t->values)
                if (v->base == TIMER_ACTIVE)
                        v->disabled = false;

        if (t->stamp_path) {
                struct stat st;

                if (stat(t->stamp_path, &st) >= 0)
                        t->last_trigger.realtime = timespec_load(&st.st_atim);
                else if (errno == ENOENT)
                         /* The timer has never run before,
                          * make sure a stamp file exists.
                          */
                        touch_file(t->stamp_path, true, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
         }
 
         t->result = TIMER_SUCCESS;
        timer_enter_waiting(t, true);
        return 1;
}
