CronTab::initRegexObject() {
	if ( ! CronTab::regex.isInitialized() ) {
		const char *errptr;
		int erroffset;
		MyString pattern( CRONTAB_PARAMETER_PATTERN ) ;
                if ( ! CronTab::regex.compile( pattern, &errptr, &erroffset )) {
                        MyString error = "CronTab: Failed to compile Regex - ";
                        error += pattern;
                       EXCEPT( "%s", const_cast<char*>(error.Value()) );
                }
        }
 }
