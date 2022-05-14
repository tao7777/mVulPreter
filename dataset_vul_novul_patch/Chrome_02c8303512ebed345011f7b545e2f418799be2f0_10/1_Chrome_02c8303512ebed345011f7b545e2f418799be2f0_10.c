 void EntrySync::remove(ExceptionState& exceptionState) const
 {
    RefPtr<VoidSyncCallbackHelper> helper = VoidSyncCallbackHelper::create();
     m_fileSystem->remove(this, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
     helper->getResult(exceptionState);
 }
