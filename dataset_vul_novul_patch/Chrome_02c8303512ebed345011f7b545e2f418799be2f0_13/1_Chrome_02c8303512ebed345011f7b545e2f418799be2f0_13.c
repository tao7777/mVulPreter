 void LocalFileSystem::fileSystemAllowedInternal(
     PassRefPtrWillBeRawPtr<ExecutionContext> context,
     FileSystemType type,
    PassRefPtr<CallbackWrapper> callbacks)
 {
     if (!fileSystem()) {
         fileSystemNotAvailable(context, callbacks);
        return;
    }

    KURL storagePartition = KURL(KURL(), context->securityOrigin()->toString());
    fileSystem()->openFileSystem(storagePartition, static_cast<WebFileSystemType>(type), callbacks->release());
}
