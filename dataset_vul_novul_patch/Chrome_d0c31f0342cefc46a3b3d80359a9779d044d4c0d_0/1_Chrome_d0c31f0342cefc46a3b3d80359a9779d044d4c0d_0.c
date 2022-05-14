 Blob::Blob(PassOwnPtr<BlobData> blobData, long long size)
    : m_type(blobData->contentType())
    , m_size(size)
{
    ASSERT(blobData);
    ScriptWrappable::init(this);
 
     m_internalURL = BlobURL::createInternalURL();
    ThreadableBlobRegistry::registerBlobURL(m_internalURL, blobData);
 }
