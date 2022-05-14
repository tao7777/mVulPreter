 I18NCustomBindings::I18NCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetL10nMessage",
       base::Bind(&I18NCustomBindings::GetL10nMessage, base::Unretained(this)));
  RouteFunction("GetL10nUILanguage",
                 base::Bind(&I18NCustomBindings::GetL10nUILanguage,
                            base::Unretained(this)));
  RouteFunction("DetectTextLanguage",
                 base::Bind(&I18NCustomBindings::DetectTextLanguage,
                            base::Unretained(this)));
 }
