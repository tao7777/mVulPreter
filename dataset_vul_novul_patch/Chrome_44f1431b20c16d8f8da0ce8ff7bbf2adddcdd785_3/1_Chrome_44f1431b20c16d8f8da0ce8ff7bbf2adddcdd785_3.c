float SVGLayoutSupport::calculateScreenFontSizeScalingFactor(const LayoutObject* layoutObject)
{
    ASSERT(layoutObject);

     AffineTransform ctm = deprecatedCalculateTransformToLayer(layoutObject) * SubtreeContentTransformScope::currentContentTransformation();
     ctm.scale(layoutObject->document().frameHost()->deviceScaleFactorDeprecated());
 
    return narrowPrecisionToFloat(sqrt((pow(ctm.xScale(), 2) + pow(ctm.yScale(), 2)) / 2));
 }
