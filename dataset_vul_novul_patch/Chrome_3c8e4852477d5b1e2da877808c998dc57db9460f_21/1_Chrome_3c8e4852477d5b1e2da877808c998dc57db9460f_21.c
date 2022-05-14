 void NetworkHandler::SetNetworkConditions(
     network::mojom::NetworkConditionsPtr conditions) {
  if (!process_)
     return;
  StoragePartition* partition = process_->GetStoragePartition();
  network::mojom::NetworkContext* context = partition->GetNetworkContext();
   context->SetNetworkConditions(host_id_, std::move(conditions));
 }
