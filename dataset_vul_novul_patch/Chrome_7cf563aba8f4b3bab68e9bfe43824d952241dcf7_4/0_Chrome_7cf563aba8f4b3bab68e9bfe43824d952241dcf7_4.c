void HostPortAllocatorSession::OnSessionRequestDone(
void HostPortAllocatorSession::OnURLFetchComplete(
    const net::URLFetcher* source) {
  url_fetchers_.erase(source);
  delete source;
 
  if (source->GetResponseCode() != net::HTTP_OK) {
     LOG(WARNING) << "Received error when allocating relay session: "
                 << source->GetResponseCode();
     TryCreateRelaySession();
     return;
   }
 
  std::string response;
  source->GetResponseAsString(&response);
   ReceiveSessionResponse(response);
 }
