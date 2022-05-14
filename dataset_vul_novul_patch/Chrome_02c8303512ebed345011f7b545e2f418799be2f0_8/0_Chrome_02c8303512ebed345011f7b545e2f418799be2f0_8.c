 Metadata* EntrySync::getMetadata(ExceptionState& exceptionState)
 {
    MetadataSyncCallbackHelper* helper = MetadataSyncCallbackHelper::create();
     m_fileSystem->getMetadata(this, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
     return helper->getResult(exceptionState);
 }
