void AwFeatureListCreator::SetUpFieldTrials() {
  auto* metrics_client = AwMetricsServiceClient::GetInstance();

  DCHECK(!field_trial_list_);
  field_trial_list_ = std::make_unique<base::FieldTrialList>(
      metrics_client->CreateLowEntropyProvider());

  std::unique_ptr<variations::SeedResponse> seed = GetAndClearJavaSeed();
  base::Time null_time;
  base::Time seed_date =
      seed ? base::Time::FromJavaTime(seed->date) : null_time;
  variations::UIStringOverrider ui_string_overrider;
  client_ = std::make_unique<AwVariationsServiceClient>();
  auto seed_store = std::make_unique<variations::VariationsSeedStore>(
      local_state_.get(), /*initial_seed=*/std::move(seed),
      /*on_initial_seed_stored=*/base::DoNothing());

  if (!seed_date.is_null())
    seed_store->RecordLastFetchTime(seed_date);

  variations_field_trial_creator_ =
      std::make_unique<variations::VariationsFieldTrialCreator>(
          local_state_.get(), client_.get(), std::move(seed_store),
          ui_string_overrider);
  variations_field_trial_creator_->OverrideVariationsPlatform(
      variations::Study::PLATFORM_ANDROID_WEBVIEW);

  std::set<std::string> unforceable_field_trials;
  variations::SafeSeedManager ignored_safe_seed_manager(true,
                                                        local_state_.get());

  variations_field_trial_creator_->SetupFieldTrials(
      cc::switches::kEnableGpuBenchmarking, switches::kEnableFeatures,
      switches::kDisableFeatures, unforceable_field_trials,
      std::vector<std::string>(),
      content::GetSwitchDependentFeatureOverrides(
           *base::CommandLine::ForCurrentProcess()),
       /*low_entropy_provider=*/nullptr, std::make_unique<base::FeatureList>(),
       aw_field_trials_.get(), &ignored_safe_seed_manager);

  CountOrRecordRestartsWithStaleSeed(local_state_.get(), IsSeedFresh());
 }
