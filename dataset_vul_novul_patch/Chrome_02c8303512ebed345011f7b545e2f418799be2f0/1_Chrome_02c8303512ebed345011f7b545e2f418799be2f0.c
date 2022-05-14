    CreateFileHelper(PassRefPtrWillBeRawPtr<CreateFileResult> result, const String& name, const KURL& url, FileSystemType type)
         : m_result(result)
         , m_name(name)
         , m_url(url)
         , m_type(type)
     {
     }
