 static void finalizeStreamTask(void* context)
 {
     OwnPtr<BlobRegistryContext> blobRegistryContext = adoptPtr(static_cast<BlobRegistryContext*>(context));
    blobRegistry().finalizeStream(blobRegistryContext->url);
 }
