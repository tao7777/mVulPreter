void RegisterOptimizationHintsComponent(ComponentUpdateService* cus,
                                        PrefService* profile_prefs) {
  if (!previews::params::IsOptimizationHintsEnabled()) {
     return;
   }
 
  if (!data_reduction_proxy::DataReductionProxySettings::
          IsDataSaverEnabledByUser(profile_prefs)) {
     return;
  }
   auto installer = base::MakeRefCounted<ComponentInstaller>(
       std::make_unique<OptimizationHintsComponentInstallerPolicy>());
   installer->Register(cus, base::OnceClosure());
}
