void ChromeMetricsServiceClient::RegisterUKMProviders() {
  ukm_service_->RegisterMetricsProvider(
      std::make_unique<metrics::NetworkMetricsProvider>(
          content::CreateNetworkConnectionTrackerAsyncGetter(),
          std::make_unique<metrics::NetworkQualityEstimatorProviderImpl>()));

#if defined(OS_CHROMEOS)
  ukm_service_->RegisterMetricsProvider(
       std::make_unique<ChromeOSMetricsProvider>());
 #endif  // !defined(OS_CHROMEOS)
 
   ukm_service_->RegisterMetricsProvider(
       std::make_unique<variations::FieldTrialsProvider>(nullptr,
                                                        kUKMFieldTrialSuffix));
}
