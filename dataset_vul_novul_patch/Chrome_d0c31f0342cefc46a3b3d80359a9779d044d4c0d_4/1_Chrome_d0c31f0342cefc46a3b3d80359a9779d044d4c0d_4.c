 Blob::~Blob()
 {
    ThreadableBlobRegistry::unregisterBlobURL(m_internalURL);
 }
