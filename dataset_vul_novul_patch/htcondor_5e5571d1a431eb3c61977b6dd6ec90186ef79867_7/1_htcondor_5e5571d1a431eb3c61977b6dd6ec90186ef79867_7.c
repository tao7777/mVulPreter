CStarter::removeDeferredJobs() {
	bool ret = true;
	
	if ( this->deferral_tid == -1 ) {
		return ( ret );
	}
	
	m_deferred_job_update = true;
	
	if ( daemonCore->Cancel_Timer( this->deferral_tid ) >= 0 ) {
		dprintf( D_FULLDEBUG, "Cancelled time deferred execution for "
							  "Job %d.%d\n", 
					this->jic->jobCluster(),
					this->jic->jobProc() );
		this->deferral_tid = -1;

	} else {
		MyString error = "Failed to cancel deferred execution timer for Job ";
                error += this->jic->jobCluster();
                error += ".";
                error += this->jic->jobProc();
               EXCEPT( error.Value() );
                ret = false;
        }
        return ( ret );
}
