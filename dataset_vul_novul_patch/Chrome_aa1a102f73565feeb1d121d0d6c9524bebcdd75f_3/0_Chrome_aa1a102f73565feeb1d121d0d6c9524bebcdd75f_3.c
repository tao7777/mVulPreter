void XMLHttpRequest::genericError()
void XMLHttpRequest::handleDidFailGeneric()
 {
     clearResponse();
     clearRequest();
 
    m_error = true;
 }
