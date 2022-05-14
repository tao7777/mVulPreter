const SystemProfileProto& MetricsLog::RecordEnvironment(
    DelegatingProvider* delegating_provider) {
  DCHECK(!has_environment_);
  has_environment_ = true;

  SystemProfileProto* system_profile = uma_proto()->mutable_system_profile();

  WriteMetricsEnableDefault(client_->GetMetricsReportingDefaultState(),
                            system_profile);

  std::string brand_code;
   if (client_->GetBrand(&brand_code))
     system_profile->set_brand_code(brand_code);
 
   delegating_provider->ProvideSystemProfileMetrics(system_profile);
 
   return *system_profile;
}
