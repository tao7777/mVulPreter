DOMFileSystemSync* WorkerGlobalScopeFileSystem::webkitRequestFileSystemSync(WorkerGlobalScope& worker, int type, long long size, ExceptionState& exceptionState)
{
    ExecutionContext* secureContext = worker.executionContext();
    if (!secureContext->securityOrigin()->canAccessFileSystem()) {
        exceptionState.throwSecurityError(FileError::securityErrorMessage);
        return 0;
    }

    FileSystemType fileSystemType = static_cast<FileSystemType>(type);
    if (!DOMFileSystemBase::isValidType(fileSystemType)) {
        exceptionState.throwDOMException(InvalidModificationError, "the type must be TEMPORARY or PERSISTENT.");
         return 0;
     }
 
    RefPtr<FileSystemSyncCallbackHelper> helper = FileSystemSyncCallbackHelper::create();
     OwnPtr<AsyncFileSystemCallbacks> callbacks = FileSystemCallbacks::create(helper->successCallback(), helper->errorCallback(), &worker, fileSystemType);
     callbacks->setShouldBlockUntilCompletion(true);
 
    LocalFileSystem::from(worker)->requestFileSystem(&worker, fileSystemType, size, callbacks.release());
    return helper->getResult(exceptionState);
}
