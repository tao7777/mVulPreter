BaseShadow::log_except(const char *msg)
{
	ShadowExceptionEvent event;
        bool exception_already_logged = false;
 
        if(!msg) msg = "";
       sprintf(event.message, "%s", msg);
 
        if ( BaseShadow::myshadow_ptr ) {
                BaseShadow *shadow = BaseShadow::myshadow_ptr;

		event.recvd_bytes = shadow->bytesSent();
		event.sent_bytes = shadow->bytesReceived();
		exception_already_logged = shadow->exception_already_logged;

		if (shadow->began_execution) {
			event.began_execution = TRUE;
		}

	} else {
		event.recvd_bytes = 0.0;
		event.sent_bytes = 0.0;
	}

	if (!exception_already_logged && !uLog.writeEventNoFsync (&event,NULL))
	{
		::dprintf (D_ALWAYS, "Unable to log ULOG_SHADOW_EXCEPTION event\n");
	}
}
