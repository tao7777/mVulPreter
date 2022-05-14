void ThreadableBlobRegistry::addDataToStream(const KURL& url, PassRefPtr<RawData> streamData)
void BlobRegistry::addDataToStream(const KURL& url, PassRefPtr<RawData> streamData)
 {
     if (isMainThread()) {
        if (WebBlobRegistry* registry = blobRegistry()) {
            WebThreadSafeData webThreadSafeData(streamData);
            registry->addDataToStream(url, webThreadSafeData);
        }
     } else {
         OwnPtr<BlobRegistryContext> context = adoptPtr(new BlobRegistryContext(url, streamData));
         callOnMainThread(&addDataToStreamTask, context.leakPtr());
    }
}
