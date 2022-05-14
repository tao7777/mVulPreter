 static void registerStreamURLTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    blobRegistry().registerStreamURL(blobRegistryContext->url, blobRegistryContext->type);
 }
