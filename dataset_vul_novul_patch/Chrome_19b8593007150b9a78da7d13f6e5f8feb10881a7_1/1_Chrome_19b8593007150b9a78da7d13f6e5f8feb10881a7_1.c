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
 
  SystemProfileProto::Hardware::CPU* cpu =
      system_profile->mutable_hardware()->mutable_cpu();
  base::CPU cpu_info;
  cpu->set_vendor_name(cpu_info.vendor_name());
  cpu->set_signature(cpu_info.signature());
  cpu->set_num_cores(base::SysInfo::NumberOfProcessors());
   delegating_provider->ProvideSystemProfileMetrics(system_profile);
 
   return *system_profile;
}
