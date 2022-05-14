void RegisterOptimizationHintsComponent(ComponentUpdateService* cus,
                                        PrefService* profile_prefs) {
  if (!previews::params::IsOptimizationHintsEnabled()) {
     return;
   }
 
  bool data_saver_enabled =
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          data_reduction_proxy::switches::kEnableDataReductionProxy) ||
      (profile_prefs && profile_prefs->GetBoolean(
                            data_reduction_proxy::prefs::kDataSaverEnabled));
  if (!data_saver_enabled)
     return;
   auto installer = base::MakeRefCounted<ComponentInstaller>(
       std::make_unique<OptimizationHintsComponentInstallerPolicy>());
   installer->Register(cus, base::OnceClosure());
}
