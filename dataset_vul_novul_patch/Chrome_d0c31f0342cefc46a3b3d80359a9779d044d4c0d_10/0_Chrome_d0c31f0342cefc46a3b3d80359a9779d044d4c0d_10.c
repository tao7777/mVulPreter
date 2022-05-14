void ThreadableBlobRegistry::registerBlobURL(SecurityOrigin* origin, const KURL& url, const KURL& srcURL)
void BlobRegistry::registerBlobURL(SecurityOrigin* origin, const KURL& url, const KURL& srcURL)
 {
     if (origin && BlobURL::getOrigin(url) == "null")
         originMap()->add(url.string(), origin);
 
    if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry())
            registry->registerBlobURL(url, srcURL);
    } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, srcURL));
         callOnMainThread(&registerBlobURLFromTask, context.leakPtr());
     }
}
