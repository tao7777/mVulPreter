void PageSerializer::addImageToResources(ImageResource* image, RenderObject* imageRenderer, const KURL& url)
{
     if (!shouldAddURL(url))
         return;
 
    if (!image || !image->hasImage() || image->image() == Image::nullImage())
         return;
 
     RefPtr<SharedBuffer> data = imageRenderer ? image->imageForRenderer(imageRenderer)->data() : 0;
    if (!data)
        data = image->image()->data();

    addToResources(image, data, url);
}
