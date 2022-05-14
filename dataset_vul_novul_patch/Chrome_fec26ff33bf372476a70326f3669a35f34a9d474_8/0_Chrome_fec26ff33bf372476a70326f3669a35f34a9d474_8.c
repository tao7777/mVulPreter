 PreresolveJob::PreresolveJob(PreconnectRequest preconnect_request,
                              PreresolveInfo* info)
    : url(preconnect_request.origin.GetURL()),
       num_sockets(preconnect_request.num_sockets),
       allow_credentials(preconnect_request.allow_credentials),
       network_isolation_key(
          std::move(preconnect_request.network_isolation_key)),
      info(info) {
  DCHECK_GE(num_sockets, 0);
}
