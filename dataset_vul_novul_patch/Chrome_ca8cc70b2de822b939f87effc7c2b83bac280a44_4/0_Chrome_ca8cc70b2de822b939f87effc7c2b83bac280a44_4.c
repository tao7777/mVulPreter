int SocketStream::HandleCertificateError(int result) {
  DCHECK(IsCertificateError(result));
   SSLClientSocket* ssl_socket = static_cast<SSLClientSocket*>(socket_.get());
   DCHECK(ssl_socket);
 
  if (!context_)
     return result;
 
   if (SSLClientSocket::IgnoreCertError(result, LOAD_IGNORE_ALL_CERT_ERRORS)) {
    const HttpNetworkSession::Params* session_params =
        context_->GetNetworkSessionParams();
    if (session_params && session_params->ignore_certificate_errors)
      return OK;
  }

  if (!delegate_)
    return result;

  SSLInfo ssl_info;
  ssl_socket->GetSSLInfo(&ssl_info);

  TransportSecurityState::DomainState domain_state;
  const bool fatal = context_->transport_security_state() &&
      context_->transport_security_state()->GetDomainState(url_.host(),
          SSLConfigService::IsSNIAvailable(context_->ssl_config_service()),
          &domain_state) &&
      domain_state.ShouldSSLErrorsBeFatal();

  delegate_->OnSSLCertificateError(this, ssl_info, fatal);
  return ERR_IO_PENDING;
}
