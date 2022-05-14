static void upnp_event_prepare(struct upnp_event_notify * obj)
{
	static const char notifymsg[] =
		"NOTIFY %s HTTP/1.1\r\n"
		"Host: %s%s\r\n"
#if (UPNP_VERSION_MAJOR == 1) && (UPNP_VERSION_MINOR == 0)
		"Content-Type: text/xml\r\n"	/* UDA v1.0 */
#else
		"Content-Type: text/xml; charset=\"utf-8\"\r\n"	/* UDA v1.1 or later */
#endif
		"Content-Length: %d\r\n"
		"NT: upnp:event\r\n"
		"NTS: upnp:propchange\r\n"
		"SID: %s\r\n"
		"SEQ: %u\r\n"
		"Connection: close\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n"
		"%.*s\r\n";
	char * xml;
	int l;
	if(obj->sub == NULL) {
		obj->state = EError;
		return;
	}
	switch(obj->sub->service) {
	case EWanCFG:
		xml = getVarsWANCfg(&l);
		break;
	case EWanIPC:
		xml = getVarsWANIPCn(&l);
		break;
#ifdef ENABLE_L3F_SERVICE
	case EL3F:
		xml = getVarsL3F(&l);
		break;
#endif
#ifdef ENABLE_6FC_SERVICE
	case E6FC:
		xml = getVars6FC(&l);
		break;
#endif
#ifdef ENABLE_DP_SERVICE
	case EDP:
		xml = getVarsDP(&l);
		break;
#endif
	default:
		xml = NULL;
 		l = 0;
 	}
 	obj->buffersize = 1024;
	for (;;) {
		obj->buffer = malloc(obj->buffersize);
		if(!obj->buffer) {
			syslog(LOG_ERR, "%s: malloc returned NULL", "upnp_event_prepare");
			if(xml) {
				free(xml);
			}
			obj->state = EError;
			return;
 		}
		obj->tosend = snprintf(obj->buffer, obj->buffersize, notifymsg,
		                       obj->path, obj->addrstr, obj->portstr, l+2,
		                       obj->sub->uuid, obj->sub->seq,
		                       l, xml);
		if (obj->tosend < 0) {
			syslog(LOG_ERR, "%s: snprintf() failed", "upnp_event_prepare");
			if(xml) {
				free(xml);
			}
			obj->state = EError;
			return;
		} else if (obj->tosend < obj->buffersize) {
			break; /* the buffer was large enough */
		}
		/* Try again with a buffer big enough */
		free(obj->buffer);
		obj->buffersize = obj->tosend + 1;	/* reserve space for the final 0 */
 	}
 	if(xml) {
 		free(xml);
 		xml = NULL;
	}
	obj->state = ESending;
}
