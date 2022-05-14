void ThreadableBlobRegistry::registerBlobURL(SecurityOrigin* origin, const KURL& url, const KURL& srcURL)
 {
     if (origin && BlobURL::getOrigin(url) == "null")
         originMap()->add(url.string(), origin);
 
    if (isMainThread())
        blobRegistry().registerBlobURL(url, srcURL);
    else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, srcURL));
         callOnMainThread(&registerBlobURLFromTask, context.leakPtr());
     }
}
