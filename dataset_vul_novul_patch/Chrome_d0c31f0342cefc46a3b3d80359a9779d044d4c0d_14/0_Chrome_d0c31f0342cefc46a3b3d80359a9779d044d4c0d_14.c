 static void registerStreamURLTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry())
        registry->registerStreamURL(blobRegistryContext->url, blobRegistryContext->type);
 }
