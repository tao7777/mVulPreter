 DisplaySourceCustomBindings::DisplaySourceCustomBindings(ScriptContext* context)
     : ObjectBackedNativeHandler(context),
       weak_factory_(this) {
  RouteFunction("StartSession", "displaySource",
                 base::Bind(&DisplaySourceCustomBindings::StartSession,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("TerminateSession", "displaySource",
                 base::Bind(&DisplaySourceCustomBindings::TerminateSession,
                            weak_factory_.GetWeakPtr()));
 }
