BlobDataHandle::BlobDataHandle(PassOwnPtr<BlobData> data, long long size)
 {
     UNUSED_PARAM(size);
     m_internalURL = BlobURL::createInternalURL();
    BlobRegistry::registerBlobURL(m_internalURL, data);
 }
