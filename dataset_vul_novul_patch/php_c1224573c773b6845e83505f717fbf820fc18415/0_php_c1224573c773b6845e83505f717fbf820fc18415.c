static time_t asn1_time_to_time_t(ASN1_UTCTIME * timestr TSRMLS_DC) /* {{{ */
{
/*
	This is how the time string is formatted:

   snprintf(p, sizeof(p), "%02d%02d%02d%02d%02d%02dZ",ts->tm_year%100,
      ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);
*/

	time_t ret;
	struct tm thetime;
	char * strbuf;
        char * thestr;
        long gmadjust = 0;
 
       if (ASN1_STRING_type(timestr) != V_ASN1_UTCTIME) {
               php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal ASN1 data type for timestamp");
                return (time_t)-1;
        }
 
       if (ASN1_STRING_length(timestr) != strlen(ASN1_STRING_data(timestr))) {
               php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal length in timestamp");
               return (time_t)-1;
       }

       if (ASN1_STRING_length(timestr) < 13) {
               php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to parse time string %s correctly", timestr->data);
               return (time_t)-1;
       }

       strbuf = estrdup((char *)ASN1_STRING_data(timestr));
 
        memset(&thetime, 0, sizeof(thetime));
 
        /* we work backwards so that we can use atoi more easily */
 
       thestr = strbuf + ASN1_STRING_length(timestr) - 3;
 
        thetime.tm_sec = atoi(thestr);
        *thestr = '\0';
	thetime.tm_mon = atoi(thestr)-1;
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_year = atoi(thestr);

	if (thetime.tm_year < 68) {
		thetime.tm_year += 100;
	}

	thetime.tm_isdst = -1;
	ret = mktime(&thetime);

#if HAVE_TM_GMTOFF
	gmadjust = thetime.tm_gmtoff;
#else
	/*
	** If correcting for daylight savings time, we set the adjustment to
	** the value of timezone - 3600 seconds. Otherwise, we need to overcorrect and
	** set the adjustment to the main timezone + 3600 seconds.
	*/
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone + 3600);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}
/* }}} */
