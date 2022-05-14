UnacceleratedStaticBitmapImage::UnacceleratedStaticBitmapImage(
     sk_sp<SkImage> image) {
   CHECK(image);
   DCHECK(!image->isLazyGenerated());
   paint_image_ =
       CreatePaintImageBuilder()
           .set_image(std::move(image), cc::PaintImage::GetNextContentId())
          .TakePaintImage();
}
