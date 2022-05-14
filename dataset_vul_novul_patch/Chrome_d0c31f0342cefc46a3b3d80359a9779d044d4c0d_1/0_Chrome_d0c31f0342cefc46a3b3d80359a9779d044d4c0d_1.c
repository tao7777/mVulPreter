 Blob::Blob(const KURL& srcURL, const String& type, long long size)
    : m_type(type)
    , m_size(size)
{
    ScriptWrappable::init(this);
 
     m_internalURL = BlobURL::createInternalURL();
    BlobRegistry::registerBlobURL(0, m_internalURL, srcURL);
 }
