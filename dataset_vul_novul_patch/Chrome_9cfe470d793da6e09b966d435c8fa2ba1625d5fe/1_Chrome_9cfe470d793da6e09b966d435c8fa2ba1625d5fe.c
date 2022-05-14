void VirtualAuthenticator::AddRegistration(
    blink::test::mojom::RegisteredKeyPtr registration,
    AddRegistrationCallback callback) {
  if (registration->application_parameter.size() != device::kRpIdHashLength) {
    std::move(callback).Run(false);
    return;
  }

  bool success = false;
  std::tie(std::ignore, success) = state_->registrations.emplace(
      std::move(registration->key_handle),
       ::device::VirtualFidoDevice::RegistrationData(
           crypto::ECPrivateKey::CreateFromPrivateKeyInfo(
               registration->private_key),
          registration->application_parameter, registration->counter));
   std::move(callback).Run(success);
 }
