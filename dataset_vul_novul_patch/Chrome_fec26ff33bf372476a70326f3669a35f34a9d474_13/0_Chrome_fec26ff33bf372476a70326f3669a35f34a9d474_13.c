 PreconnectRequest::PreconnectRequest(
    const url::Origin& origin,
     int num_sockets,
     const net::NetworkIsolationKey& network_isolation_key)
     : origin(origin),
      num_sockets(num_sockets),
      network_isolation_key(network_isolation_key) {
  DCHECK_GE(num_sockets, 0);
}
