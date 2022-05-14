float SVGLayoutSupport::calculateScreenFontSizeScalingFactor(const LayoutObject* layoutObject)
{
    ASSERT(layoutObject);

     AffineTransform ctm = deprecatedCalculateTransformToLayer(layoutObject) * SubtreeContentTransformScope::currentContentTransformation();
     ctm.scale(layoutObject->document().frameHost()->deviceScaleFactorDeprecated());
 
    return narrowPrecisionToFloat(sqrt((ctm.xScaleSquared() + ctm.yScaleSquared()) / 2));
 }
