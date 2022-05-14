void XMLHttpRequest::genericError()
 {
     clearResponse();
     clearRequest();
    m_error = true;
 
    changeState(DONE);
 }
