   static bool IsManualFallbackForFillingEnabled() {
     return base::FeatureList::IsEnabled(
               password_manager::features::kManualFallbacksFilling) &&
            !IsPreLollipopAndroid();
   }
