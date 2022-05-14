void ImageBitmapFactories::ImageBitmapLoader::LoadBlobAsync(
void ImageBitmapFactories::ImageBitmapLoader::LoadBlobAsync(Blob* blob) {
   loader_->Start(blob->GetBlobDataHandle());
 }
