 static void addDataToStreamTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry()) {
        WebThreadSafeData webThreadSafeData(blobRegistryContext->streamData);
        registry->addDataToStream(blobRegistryContext->url, webThreadSafeData);
    }
 }
