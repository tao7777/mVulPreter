 TestFeaturesNativeHandler::TestFeaturesNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
  RouteFunction("GetAPIFeatures", "test",
                 base::Bind(&TestFeaturesNativeHandler::GetAPIFeatures,
                            base::Unretained(this)));
 }
