void BaseRenderingContext2D::setFillStyle(
    const StringOrCanvasGradientOrCanvasPattern& style) {
  DCHECK(!style.IsNull());
  ValidateStateStack();
  String color_string;
  CanvasStyle* canvas_style = nullptr;
  if (style.IsString()) {
    color_string = style.GetAsString();
    if (color_string == GetState().UnparsedFillColor())
      return;
    Color parsed_color = 0;
    if (!ParseColorOrCurrentColor(parsed_color, color_string))
      return;
    if (GetState().FillStyle()->IsEquivalentRGBA(parsed_color.Rgb())) {
      ModifiableState().SetUnparsedFillColor(color_string);
      return;
    }
    canvas_style = CanvasStyle::CreateFromRGBA(parsed_color.Rgb());
  } else if (style.IsCanvasGradient()) {
    canvas_style = CanvasStyle::CreateFromGradient(style.GetAsCanvasGradient());
   } else if (style.IsCanvasPattern()) {
     CanvasPattern* canvas_pattern = style.GetAsCanvasPattern();
 
    if (OriginClean() && !canvas_pattern->OriginClean()) {
      SetOriginTainted();
      ClearResolvedFilters();
     }
     if (canvas_pattern->GetPattern()->IsTextureBacked())
       DisableDeferral(kDisableDeferralReasonUsingTextureBackedPattern);
    canvas_style = CanvasStyle::CreateFromPattern(canvas_pattern);
  }

  DCHECK(canvas_style);
  ModifiableState().SetFillStyle(canvas_style);
  ModifiableState().SetUnparsedFillColor(color_string);
  ModifiableState().ClearResolvedFilter();
}
