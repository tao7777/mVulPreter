 I18NCustomBindings::I18NCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetL10nMessage", "i18n",
       base::Bind(&I18NCustomBindings::GetL10nMessage, base::Unretained(this)));
  RouteFunction("GetL10nUILanguage", "i18n",
                 base::Bind(&I18NCustomBindings::GetL10nUILanguage,
                            base::Unretained(this)));
  RouteFunction("DetectTextLanguage", "i18n",
                 base::Bind(&I18NCustomBindings::DetectTextLanguage,
                            base::Unretained(this)));
 }
