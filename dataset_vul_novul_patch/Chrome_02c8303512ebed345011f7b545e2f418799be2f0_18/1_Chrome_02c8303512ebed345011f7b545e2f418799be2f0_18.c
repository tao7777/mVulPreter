 void LocalFileSystem::resolveURLInternal(
     PassRefPtrWillBeRawPtr<ExecutionContext> context,
     const KURL& fileSystemURL,
    PassRefPtr<CallbackWrapper> callbacks)
 {
     if (!fileSystem()) {
         fileSystemNotAvailable(context, callbacks);
        return;
    }
    fileSystem()->resolveURL(fileSystemURL, callbacks->release());
}
