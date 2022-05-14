void ThreadableBlobRegistry::registerBlobURL(const KURL& url, PassOwnPtr<BlobData> blobData)
void BlobRegistry::registerBlobURL(const KURL& url, PassOwnPtr<BlobData> blobData)
 {
    if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry()) {
            WebBlobData webBlobData(blobData);
            registry->registerBlobURL(url, webBlobData);
        }
    } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, blobData));
         callOnMainThread(&registerBlobURLTask, context.leakPtr());
     }
}
