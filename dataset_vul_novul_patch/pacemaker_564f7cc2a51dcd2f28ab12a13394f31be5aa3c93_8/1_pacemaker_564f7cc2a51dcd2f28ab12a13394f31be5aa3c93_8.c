cib_timeout_handler(gpointer data)
{
    struct timer_rec_s *timer = data;
    timer_expired = TRUE;
    crm_err("Call %d timed out after %ds", timer->call_id, timer->timeout);
    /* Always return TRUE, never remove the handler
     * We do that after the while-loop in cib_native_perform_op()
     */
    return TRUE;
}
