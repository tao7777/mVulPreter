void ThreadableBlobRegistry::finalizeStream(const KURL& url)
 {
     if (isMainThread()) {
        blobRegistry().finalizeStream(url);
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url));
         callOnMainThread(&finalizeStreamTask, context.leakPtr());
    }
}
