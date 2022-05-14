void ThreadableBlobRegistry::unregisterBlobURL(const KURL& url)
void BlobRegistry::unregisterBlobURL(const KURL& url)
 {
     if (BlobURL::getOrigin(url) == "null")
         originMap()->remove(url.string());
 
    if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry())
            registry->unregisterBlobURL(url);
    } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url));
         callOnMainThread(&unregisterBlobURLTask, context.leakPtr());
     }
}
