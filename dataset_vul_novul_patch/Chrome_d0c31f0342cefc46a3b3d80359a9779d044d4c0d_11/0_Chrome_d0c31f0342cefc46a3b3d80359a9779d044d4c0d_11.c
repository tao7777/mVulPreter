 static void registerBlobURLFromTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry())
        registry->registerBlobURL(blobRegistryContext->url, blobRegistryContext->srcURL);
 }
