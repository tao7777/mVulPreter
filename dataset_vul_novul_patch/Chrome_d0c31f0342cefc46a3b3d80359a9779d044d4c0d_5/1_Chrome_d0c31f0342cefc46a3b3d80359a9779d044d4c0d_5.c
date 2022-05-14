void ThreadableBlobRegistry::addDataToStream(const KURL& url, PassRefPtr<RawData> streamData)
 {
     if (isMainThread()) {
        blobRegistry().addDataToStream(url, streamData);
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, streamData));
         callOnMainThread(&addDataToStreamTask, context.leakPtr());
    }
}
