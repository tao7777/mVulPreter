   void SetManualFallbacks(bool enabled) {
     std::vector<std::string> features = {
        password_manager::features::kManualFallbacksFilling.name,
         password_manager::features::kEnableManualFallbacksFillingStandalone
             .name,
         password_manager::features::kEnableManualFallbacksGeneration.name};
    if (enabled) {
      scoped_feature_list_.InitFromCommandLine(base::JoinString(features, ","),
                                               std::string());
    } else {
      scoped_feature_list_.InitFromCommandLine(std::string(),
                                               base::JoinString(features, ","));
    }
  }
