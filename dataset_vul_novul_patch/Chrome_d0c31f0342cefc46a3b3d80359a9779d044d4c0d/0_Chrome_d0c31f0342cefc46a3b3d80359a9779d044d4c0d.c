Blob::Blob()
    : m_size(0)
{
    ScriptWrappable::init(this);
    OwnPtr<BlobData> blobData = BlobData::create();
 
     m_internalURL = BlobURL::createInternalURL();
    BlobRegistry::registerBlobURL(m_internalURL, blobData.release());
 }
