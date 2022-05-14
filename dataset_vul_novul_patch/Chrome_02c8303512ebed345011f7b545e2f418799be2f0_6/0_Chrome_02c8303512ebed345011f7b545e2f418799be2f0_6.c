 void DirectoryEntrySync::removeRecursively(ExceptionState& exceptionState)
 {
    VoidSyncCallbackHelper* helper = VoidSyncCallbackHelper::create();
     m_fileSystem->removeRecursively(this, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
     helper->getResult(exceptionState);
 }
