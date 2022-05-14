int MockNetworkTransaction::RestartWithAuth(
    const AuthCredentials& credentials,
    const CompletionCallback& callback) {
  if (!IsReadyToRestartForAuth())
     return ERR_FAILED;
 
   HttpRequestInfo auth_request_info = *request_;
  auth_request_info.extra_headers.SetHeader("Authorization", "Bar");
 
  return StartInternal(&auth_request_info, callback, BoundNetLog());
}
