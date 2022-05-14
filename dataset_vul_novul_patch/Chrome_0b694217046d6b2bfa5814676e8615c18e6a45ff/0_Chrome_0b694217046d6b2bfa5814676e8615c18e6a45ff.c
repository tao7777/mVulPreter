void SystemClipboard::WriteImage(Image* image,
                                 const KURL& url,
                                 const String& title) {
  DCHECK(image);

  PaintImage paint_image = image->PaintImageForCurrentFrame();
  SkBitmap bitmap;
  if (sk_sp<SkImage> sk_image = paint_image.GetSkImage())
    sk_image->asLegacyBitmap(&bitmap);
   if (bitmap.isNull())
     return;
 
  // TODO(crbug.com/918717): Remove CHECK if no crashes occur on it in canary.
  CHECK(bitmap.getPixels());
 
   clipboard_->WriteImage(mojom::ClipboardBuffer::kStandard, bitmap);
 
  if (url.IsValid() && !url.IsEmpty()) {
#if !defined(OS_MACOSX)
    clipboard_->WriteBookmark(mojom::ClipboardBuffer::kStandard,
                              url.GetString(), NonNullString(title));
#endif

    clipboard_->WriteHtml(mojom::ClipboardBuffer::kStandard,
                          URLToImageMarkup(url, title), KURL());
  }
  clipboard_->CommitWrite(mojom::ClipboardBuffer::kStandard);
}
