 void LocalFileSystem::resolveURL(ExecutionContext* context, const KURL& fileSystemURL, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
 {
     RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
    RefPtr<CallbackWrapper> wrapper = adoptRef(new CallbackWrapper(callbacks));
     requestFileSystemAccessInternal(context,
         bind(&LocalFileSystem::resolveURLInternal, this, contextPtr, fileSystemURL, wrapper),
         bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}
