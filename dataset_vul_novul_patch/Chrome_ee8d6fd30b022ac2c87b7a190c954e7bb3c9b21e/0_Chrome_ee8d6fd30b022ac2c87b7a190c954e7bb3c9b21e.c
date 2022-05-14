bool AeroPeekManager::GetTabPreview(int tab_id, SkBitmap* preview) {
  DCHECK(preview);

  TabContents* contents = GetTabContents(tab_id);
  if (!contents)
    return false;

  RenderViewHost* render_view_host = contents->render_view_host();
  if (!render_view_host)
    return false;

  BackingStore* backing_store = render_view_host->GetBackingStore(false);
  if (!backing_store)
    return false;

   skia::PlatformCanvas canvas;
  if (!backing_store->CopyFromBackingStore(gfx::Rect(backing_store->size()),
                                            &canvas))
     return false;
 
  const SkBitmap& bitmap = canvas.getTopPlatformDevice().accessBitmap(false);
  bitmap.copyTo(preview, SkBitmap::kARGB_8888_Config);
  return true;
}
