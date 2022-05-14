 void BlobURLRegistry::registerURL(SecurityOrigin* origin, const KURL& publicURL, URLRegistrable* blob)
 {
     ASSERT(&blob->registry() == this);
    ThreadableBlobRegistry::registerBlobURL(origin, publicURL, static_cast<Blob*>(blob)->url());
 }
