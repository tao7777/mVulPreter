void DocumentModuleScriptFetcher::NotifyFinished(Resource* resource) {
  ClearResource();

  ScriptResource* script_resource = ToScriptResource(resource);

  HeapVector<Member<ConsoleMessage>> error_messages;
  if (!WasModuleLoadSuccessful(script_resource, &error_messages)) {
    Finalize(WTF::nullopt, error_messages);
    return;
  }

   ModuleScriptCreationParams params(
       script_resource->GetResponse().Url(), script_resource->SourceText(),
       script_resource->GetResourceRequest().GetFetchCredentialsMode(),
      script_resource->CalculateAccessControlStatus(
          fetcher_->Context().GetSecurityOrigin()));
   Finalize(params, error_messages);
 }
