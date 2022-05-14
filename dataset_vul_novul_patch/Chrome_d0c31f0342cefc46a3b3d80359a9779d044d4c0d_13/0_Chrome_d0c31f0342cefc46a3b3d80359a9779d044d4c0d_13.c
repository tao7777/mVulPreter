void ThreadableBlobRegistry::registerStreamURL(const KURL& url, const String& type)
void BlobRegistry::registerStreamURL(const KURL& url, const String& type)
 {
     if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry())
            registry->registerStreamURL(url, type);
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, type));
         callOnMainThread(&registerStreamURLTask, context.leakPtr());
    }
}
