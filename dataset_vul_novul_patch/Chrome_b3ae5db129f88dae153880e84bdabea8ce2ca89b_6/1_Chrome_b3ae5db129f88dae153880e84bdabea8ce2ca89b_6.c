NetworkLibrary* CrosLibrary::GetNetworkLibrary() {
  return network_lib_.GetDefaultImpl(use_stub_impl_);
}
