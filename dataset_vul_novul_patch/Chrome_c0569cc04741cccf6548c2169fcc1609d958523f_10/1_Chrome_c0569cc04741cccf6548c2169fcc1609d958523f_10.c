 TestFeaturesNativeHandler::TestFeaturesNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("GetAPIFeatures",
                 base::Bind(&TestFeaturesNativeHandler::GetAPIFeatures,
                            base::Unretained(this)));
 }
