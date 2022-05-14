    static PassOwnPtr<AsyncFileSystemCallbacks> create(PassRefPtrWillBeRawPtr<CreateFileResult> result, const String& name, const KURL& url, FileSystemType type)
    static PassOwnPtr<AsyncFileSystemCallbacks> create(CreateFileResult* result, const String& name, const KURL& url, FileSystemType type)
     {
         return adoptPtr(static_cast<AsyncFileSystemCallbacks*>(new CreateFileHelper(result, name, url, type)));
     }
