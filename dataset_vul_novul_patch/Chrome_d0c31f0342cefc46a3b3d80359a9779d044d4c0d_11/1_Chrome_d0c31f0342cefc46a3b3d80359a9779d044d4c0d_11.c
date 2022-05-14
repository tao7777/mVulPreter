 static void registerBlobURLFromTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    blobRegistry().registerBlobURL(blobRegistryContext->url, blobRegistryContext->srcURL);
 }
