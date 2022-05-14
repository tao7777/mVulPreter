 void DirectoryEntrySync::removeRecursively(ExceptionState& exceptionState)
 {
    RefPtr<VoidSyncCallbackHelper> helper = VoidSyncCallbackHelper::create();
     m_fileSystem->removeRecursively(this, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
     helper->getResult(exceptionState);
 }
