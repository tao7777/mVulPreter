 static void unregisterBlobURLTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry())
        registry->unregisterBlobURL(blobRegistryContext->url);
 }
