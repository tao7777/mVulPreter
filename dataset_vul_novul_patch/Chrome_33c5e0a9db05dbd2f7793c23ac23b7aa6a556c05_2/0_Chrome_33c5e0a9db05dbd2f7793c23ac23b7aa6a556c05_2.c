   void SetManualFallbacksForFilling(bool enabled) {
     if (enabled) {
       scoped_feature_list_.InitAndEnableFeature(
          password_manager::features::kManualFallbacksFilling);
     } else {
       scoped_feature_list_.InitAndDisableFeature(
          password_manager::features::kManualFallbacksFilling);
     }
   }
