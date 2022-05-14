void ThreadableBlobRegistry::registerStreamURL(const KURL& url, const String& type)
 {
     if (isMainThread()) {
        blobRegistry().registerStreamURL(url, type);
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, type));
         callOnMainThread(&registerStreamURLTask, context.leakPtr());
    }
}
