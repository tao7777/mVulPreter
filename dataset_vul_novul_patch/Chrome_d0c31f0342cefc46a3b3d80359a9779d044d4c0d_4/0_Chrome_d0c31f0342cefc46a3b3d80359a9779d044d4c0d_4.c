 Blob::~Blob()
 {
    BlobRegistry::unregisterBlobURL(m_internalURL);
 }
