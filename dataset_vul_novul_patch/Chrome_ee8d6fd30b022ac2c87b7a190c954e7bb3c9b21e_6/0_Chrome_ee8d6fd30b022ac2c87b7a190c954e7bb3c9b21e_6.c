SkBitmap GetThumbnailForBackingStore(BackingStore* backing_store) {
  base::TimeTicks begin_compute_thumbnail = base::TimeTicks::Now();

  SkBitmap result;

   skia::PlatformCanvas temp_canvas;
  if (!backing_store->CopyFromBackingStore(gfx::Rect(backing_store->size()),
                                            &temp_canvas))
     return result;
   const SkBitmap& bmp = temp_canvas.getTopPlatformDevice().accessBitmap(false);

#if defined(OS_CHROMEOS)
  result = SkBitmapOperations::DownsampleByTwo(bmp);
#else
  result = SkBitmapOperations::DownsampleByTwoUntilSize(bmp,
                                                        kThumbnailWidth,
                                                        kThumbnailHeight);

  if (bmp.width() == result.width() &&
      bmp.height() == result.height())
    bmp.copyTo(&result, SkBitmap::kARGB_8888_Config);
#endif

  HISTOGRAM_TIMES(kThumbnailHistogramName,
                  base::TimeTicks::Now() - begin_compute_thumbnail);
  return result;
}
