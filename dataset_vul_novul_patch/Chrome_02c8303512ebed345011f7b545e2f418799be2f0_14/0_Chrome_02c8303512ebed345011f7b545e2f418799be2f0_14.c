 void LocalFileSystem::fileSystemNotAllowedInternal(
     PassRefPtrWillBeRawPtr<ExecutionContext> context,
    CallbackWrapper* callbacks)
 {
     context->postTask(createCrossThreadTask(&reportFailure, callbacks->release(), FileError::ABORT_ERR));
 }
