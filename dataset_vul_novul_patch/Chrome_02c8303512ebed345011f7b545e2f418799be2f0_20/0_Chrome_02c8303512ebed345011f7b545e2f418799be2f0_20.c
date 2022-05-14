EntrySync* WorkerGlobalScopeFileSystem::webkitResolveLocalFileSystemSyncURL(WorkerGlobalScope& worker, const String& url, ExceptionState& exceptionState)
{
    KURL completedURL = worker.completeURL(url);
    ExecutionContext* secureContext = worker.executionContext();
    if (!secureContext->securityOrigin()->canAccessFileSystem() || !secureContext->securityOrigin()->canRequest(completedURL)) {
        exceptionState.throwSecurityError(FileError::securityErrorMessage);
        return 0;
    }

    if (!completedURL.isValid()) {
        exceptionState.throwDOMException(EncodingError, "the URL '" + url + "' is invalid.");
         return 0;
     }
 
    EntrySyncCallbackHelper* resolveURLHelper = EntrySyncCallbackHelper::create();
     OwnPtr<AsyncFileSystemCallbacks> callbacks = ResolveURICallbacks::create(resolveURLHelper->successCallback(), resolveURLHelper->errorCallback(), &worker);
     callbacks->setShouldBlockUntilCompletion(true);
 
    LocalFileSystem::from(worker)->resolveURL(&worker, completedURL, callbacks.release());

    return resolveURLHelper->getResult(exceptionState);
}
