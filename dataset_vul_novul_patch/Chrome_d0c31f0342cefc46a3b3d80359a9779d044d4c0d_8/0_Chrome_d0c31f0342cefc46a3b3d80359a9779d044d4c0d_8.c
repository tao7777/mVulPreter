 static void finalizeStreamTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry())
        registry->finalizeStream(blobRegistryContext->url);
 }
