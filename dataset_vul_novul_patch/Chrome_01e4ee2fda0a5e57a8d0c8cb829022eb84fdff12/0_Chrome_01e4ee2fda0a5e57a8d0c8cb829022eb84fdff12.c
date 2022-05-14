static PassRefPtr<CSSValue> getPositionOffsetValue(RenderStyle* style, CSSPropertyID propertyID, RenderView* renderView)
{
    if (!style)
        return 0;

    Length l;
    switch (propertyID) {
        case CSSPropertyLeft:
            l = style->left();
            break;
        case CSSPropertyRight:
            l = style->right();
            break;
        case CSSPropertyTop:
            l = style->top();
            break;
        case CSSPropertyBottom:
            l = style->bottom();
            break;
        default:
            return 0;
    }

    if (style->position() == AbsolutePosition || style->position() == FixedPosition) {
        if (l.type() == WebCore::Fixed)
            return zoomAdjustedPixelValue(l.value(), style);
        else if (l.isViewportPercentage())
            return zoomAdjustedPixelValue(valueForLength(l, 0, renderView), style);
         return cssValuePool().createValue(l);
     }
 
    if (style->position() == RelativePosition) {
         return cssValuePool().createValue(l);
    }
 
     return cssValuePool().createIdentifierValue(CSSValueAuto);
 }
