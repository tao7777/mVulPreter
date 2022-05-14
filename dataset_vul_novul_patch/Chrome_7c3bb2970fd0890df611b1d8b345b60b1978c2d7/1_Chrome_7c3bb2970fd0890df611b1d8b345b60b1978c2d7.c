bool PlatformFontSkia::InitDefaultFont() {
  if (g_default_font.Get())
    return true;
 
   bool success = false;
   std::string family = kFallbackFontFamilyName;
  int size_pixels = 12;
   int style = Font::NORMAL;
   Font::Weight weight = Font::Weight::NORMAL;
   FontRenderParams params;

  const SkiaFontDelegate* delegate = SkiaFontDelegate::instance();
  if (delegate) {
    delegate->GetDefaultFontDescription(&family, &size_pixels, &style, &weight,
                                        &params);
  } else if (default_font_description_) {
#if defined(OS_CHROMEOS)
    FontRenderParamsQuery query;
    CHECK(FontList::ParseDescription(*default_font_description_,
                                     &query.families, &query.style,
                                     &query.pixel_size, &query.weight))
        << "Failed to parse font description " << *default_font_description_;
    params = gfx::GetFontRenderParams(query, &family);
    size_pixels = query.pixel_size;
    style = query.style;
    weight = query.weight;
#else
    NOTREACHED();
#endif
  }

  sk_sp<SkTypeface> typeface =
      CreateSkTypeface(style & Font::ITALIC, weight, &family, &success);
  if (!success)
    return false;
  g_default_font.Get() = new PlatformFontSkia(
      std::move(typeface), family, size_pixels, style, weight, params);
  return true;
}
