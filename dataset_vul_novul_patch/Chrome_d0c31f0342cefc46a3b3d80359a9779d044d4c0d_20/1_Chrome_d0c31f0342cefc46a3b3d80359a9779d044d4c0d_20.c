 BlobDataHandle::~BlobDataHandle()
 {
    ThreadableBlobRegistry::unregisterBlobURL(m_internalURL);
 }
