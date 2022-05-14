void RenderSVGImage::paint(PaintInfo& paintInfo, const LayoutPoint&)
{
    ANNOTATE_GRAPHICS_CONTEXT(paintInfo, this);

    if (paintInfo.context->paintingDisabled() || style()->visibility() == HIDDEN || !m_imageResource->hasImage())
        return;

    FloatRect boundingBox = repaintRectInLocalCoordinates();
    if (!SVGRenderSupport::paintInfoIntersectsRepaintRect(boundingBox, m_localTransform, paintInfo))
        return;

    PaintInfo childPaintInfo(paintInfo);
    bool drawsOutline = style()->outlineWidth() && (childPaintInfo.phase == PaintPhaseOutline || childPaintInfo.phase == PaintPhaseSelfOutline);
    if (drawsOutline || childPaintInfo.phase == PaintPhaseForeground) {
         GraphicsContextStateSaver stateSaver(*childPaintInfo.context);
         childPaintInfo.applyTransform(m_localTransform);
 
        if (childPaintInfo.phase == PaintPhaseForeground && !m_objectBoundingBox.isEmpty()) {
             SVGRenderingContext renderingContext(this, childPaintInfo);
 
             if (renderingContext.isRenderingPrepared()) {
                if (style()->svgStyle()->bufferedRendering() == BR_STATIC && renderingContext.bufferForeground(m_bufferedForeground))
                     return;
 
                 paintForeground(childPaintInfo);
            }
        }

        if (drawsOutline)
            paintOutline(childPaintInfo, IntRect(boundingBox));
    }
}
