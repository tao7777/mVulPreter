 void LocalFileSystem::requestFileSystem(ExecutionContext* context, FileSystemType type, long long size, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
 {
     RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
    CallbackWrapper* wrapper = new CallbackWrapper(callbacks);
     requestFileSystemAccessInternal(context,
         bind(&LocalFileSystem::fileSystemAllowedInternal, this, contextPtr, type, wrapper),
         bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}
