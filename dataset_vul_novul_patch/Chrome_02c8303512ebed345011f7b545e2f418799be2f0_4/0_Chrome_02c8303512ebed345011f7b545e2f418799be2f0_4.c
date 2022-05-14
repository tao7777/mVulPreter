 DirectoryEntrySync* DirectoryEntrySync::getDirectory(const String& path, const Dictionary& options, ExceptionState& exceptionState)
 {
     FileSystemFlags flags(options);
    EntrySyncCallbackHelper* helper = EntrySyncCallbackHelper::create();
     m_fileSystem->getDirectory(this, path, flags, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
     return static_cast<DirectoryEntrySync*>(helper->getResult(exceptionState));
 }
