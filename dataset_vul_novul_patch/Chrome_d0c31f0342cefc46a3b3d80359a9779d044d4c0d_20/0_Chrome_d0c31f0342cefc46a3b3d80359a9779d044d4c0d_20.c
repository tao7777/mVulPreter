 BlobDataHandle::~BlobDataHandle()
 {
    BlobRegistry::unregisterBlobURL(m_internalURL);
 }
