   void CreateAuthenticatorFactory() {
     DCHECK(context_->network_task_runner()->BelongsToCurrentThread());
 
    if (!host_ || shutting_down_)
      return;

     std::string local_certificate = key_pair_.GenerateCertificate();
     if (local_certificate.empty()) {
       LOG(ERROR) << "Failed to generate host certificate.";
      Shutdown(kHostInitializationFailed);
      return;
    }

    scoped_ptr<protocol::AuthenticatorFactory> factory(
        new protocol::Me2MeHostAuthenticatorFactory(
            local_certificate, *key_pair_.private_key(), host_secret_hash_));
    host_->SetAuthenticatorFactory(factory.Pass());
  }
