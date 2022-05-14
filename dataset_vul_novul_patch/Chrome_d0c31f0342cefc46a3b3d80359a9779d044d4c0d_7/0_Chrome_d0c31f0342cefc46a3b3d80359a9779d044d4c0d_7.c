void ThreadableBlobRegistry::finalizeStream(const KURL& url)
void BlobRegistry::finalizeStream(const KURL& url)
 {
     if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry())
            registry->finalizeStream(url);
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url));
         callOnMainThread(&finalizeStreamTask, context.leakPtr());
    }
}
