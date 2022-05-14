 static void registerBlobURLTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    blobRegistry().registerBlobURL(blobRegistryContext->url, blobRegistryContext->blobData.release());
 }
