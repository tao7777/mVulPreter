void BaseRenderingContext2D::setStrokeStyle(
    const StringOrCanvasGradientOrCanvasPattern& style) {
  DCHECK(!style.IsNull());

  String color_string;
  CanvasStyle* canvas_style = nullptr;
  if (style.IsString()) {
    color_string = style.GetAsString();
    if (color_string == GetState().UnparsedStrokeColor())
      return;
    Color parsed_color = 0;
    if (!ParseColorOrCurrentColor(parsed_color, color_string))
      return;
    if (GetState().StrokeStyle()->IsEquivalentRGBA(parsed_color.Rgb())) {
      ModifiableState().SetUnparsedStrokeColor(color_string);
      return;
    }
    canvas_style = CanvasStyle::CreateFromRGBA(parsed_color.Rgb());
  } else if (style.IsCanvasGradient()) {
    canvas_style = CanvasStyle::CreateFromGradient(style.GetAsCanvasGradient());
   } else if (style.IsCanvasPattern()) {
     CanvasPattern* canvas_pattern = style.GetAsCanvasPattern();
 
    if (!origin_tainted_by_content_ && !canvas_pattern->OriginClean())
      SetOriginTaintedByContent();
 
     canvas_style = CanvasStyle::CreateFromPattern(canvas_pattern);
   }

  DCHECK(canvas_style);

  ModifiableState().SetStrokeStyle(canvas_style);
  ModifiableState().SetUnparsedStrokeColor(color_string);
  ModifiableState().ClearResolvedFilter();
}
