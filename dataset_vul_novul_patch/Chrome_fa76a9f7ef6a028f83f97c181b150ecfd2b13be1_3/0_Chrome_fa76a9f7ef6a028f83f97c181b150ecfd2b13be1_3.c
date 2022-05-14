void CoordinatorImpl::RegisterClientProcess(
    mojom::ClientProcessPtr client_process_ptr,
    mojom::ProcessType process_type) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   mojom::ClientProcess* client_process = client_process_ptr.get();
   client_process_ptr.set_connection_error_handler(
       base::BindOnce(&CoordinatorImpl::UnregisterClientProcess,
                     weak_ptr_factory_.GetWeakPtr(), client_process));
   auto identity = GetClientIdentityForCurrentRequest();
   auto client_info = std::make_unique<ClientInfo>(
       std::move(identity), std::move(client_process_ptr), process_type);
  auto iterator_and_inserted =
      clients_.emplace(client_process, std::move(client_info));
  DCHECK(iterator_and_inserted.second);
}
