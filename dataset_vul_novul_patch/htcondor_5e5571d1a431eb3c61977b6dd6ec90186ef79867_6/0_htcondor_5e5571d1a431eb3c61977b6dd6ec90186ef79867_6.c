pseudo_ulog( ClassAd *ad )
{
	ULogEvent *event = instantiateEvent(ad);
	int result = 0;
	char const *critical_error = NULL;
	MyString CriticalErrorBuf;
	bool event_already_logged = false;
	bool put_job_on_hold = false;
	char const *hold_reason = NULL;
	char *hold_reason_buf = NULL;
	int hold_reason_code = 0;
	int hold_reason_sub_code = 0;

	if(!event) {
		MyString add_str;
		ad->sPrint(add_str);
		dprintf(
		  D_ALWAYS,
		  "invalid event ClassAd in pseudo_ulog: %s\n",
		  add_str.Value());
		return -1;
	}

	if(ad->LookupInteger(ATTR_HOLD_REASON_CODE,hold_reason_code)) {
		put_job_on_hold = true;
		ad->LookupInteger(ATTR_HOLD_REASON_SUBCODE,hold_reason_sub_code);
		ad->LookupString(ATTR_HOLD_REASON,&hold_reason_buf);
		if(hold_reason_buf) {
			hold_reason = hold_reason_buf;
		}
	}

	if( event->eventNumber == ULOG_REMOTE_ERROR ) {
		RemoteErrorEvent *err = (RemoteErrorEvent *)event;

		if(!err->getExecuteHost() || !*err->getExecuteHost()) {
			char *execute_host = NULL;
			thisRemoteResource->getMachineName(execute_host);
			err->setExecuteHost(execute_host);
			delete[] execute_host;
		}

		if(err->isCriticalError()) {
			CriticalErrorBuf.sprintf(
			  "Error from %s: %s",
			  err->getExecuteHost(),
			  err->getErrorText());

			critical_error = CriticalErrorBuf.Value();
			if(!hold_reason) {
				hold_reason = critical_error;
			}

			BaseShadow::log_except(critical_error);
			event_already_logged = true;
		}
	}

	if( !event_already_logged && !Shadow->uLog.writeEvent( event, ad ) ) {
		MyString add_str;
		ad->sPrint(add_str);
		dprintf(
		  D_ALWAYS,
		  "unable to log event in pseudo_ulog: %s\n",
		  add_str.Value());
		result = -1;
	}

	if(put_job_on_hold) {
		hold_reason = critical_error;
		if(!hold_reason) {
			hold_reason = "Job put on hold by remote host.";
		}
		Shadow->holdJobAndExit(hold_reason,hold_reason_code,hold_reason_sub_code);
	}

	if( critical_error ) {
                Shadow->exception_already_logged = true;
 
               EXCEPT("%s", critical_error);
        }
 
        delete event;
	return result;
}
