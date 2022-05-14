 void NetworkHandler::SetNetworkConditions(
     network::mojom::NetworkConditionsPtr conditions) {
  if (!storage_partition_)
     return;
  network::mojom::NetworkContext* context =
      storage_partition_->GetNetworkContext();
   context->SetNetworkConditions(host_id_, std::move(conditions));
 }
