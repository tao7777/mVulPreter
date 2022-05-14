void SocketStreamDispatcherHost::OnSSLCertificateError(
    net::SocketStream* socket, const net::SSLInfo& ssl_info, bool fatal) {
  int socket_id = SocketStreamHost::SocketIdFromSocketStream(socket);
  DVLOG(1) << "SocketStreamDispatcherHost::OnSSLCertificateError socket_id="
           << socket_id;
  if (socket_id == content::kNoSocketId) {
    LOG(ERROR) << "NoSocketId in OnSSLCertificateError";
    return;
  }
   SocketStreamHost* socket_stream_host = hosts_.Lookup(socket_id);
   DCHECK(socket_stream_host);
   content::GlobalRequestID request_id(-1, socket_id);
  SSLManager::OnSSLCertificateError(ssl_delegate_weak_factory_.GetWeakPtr(),
      request_id, ResourceType::SUB_RESOURCE, socket->url(),
       render_process_id_, socket_stream_host->render_view_id(), ssl_info,
       fatal);
 }
