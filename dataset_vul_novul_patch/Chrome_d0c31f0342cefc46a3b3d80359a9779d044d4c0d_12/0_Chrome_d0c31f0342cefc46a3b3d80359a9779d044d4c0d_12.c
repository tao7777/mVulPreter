 static void registerBlobURLTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    if (WebBlobRegistry* registry = blobRegistry()) {
        WebBlobData webBlobData(blobRegistryContext->blobData.release());
        registry->registerBlobURL(blobRegistryContext->url, webBlobData);
    }
 }
