void RenderBox::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
    bool oldHorizontalWritingMode = isHorizontalWritingMode();

    RenderBoxModelObject::styleDidChange(diff, oldStyle);

    RenderStyle* newStyle = style();
    if (needsLayout() && oldStyle) {
        RenderBlock::removePercentHeightDescendantIfNeeded(this);

        if (isOutOfFlowPositioned() && newStyle->hasStaticBlockPosition(isHorizontalWritingMode()) && oldStyle->marginBefore() != newStyle->marginBefore()
            && parent() && !parent()->normalChildNeedsLayout())
            parent()->setChildNeedsLayout();
    }

    if (RenderBlock::hasPercentHeightContainerMap() && firstChild()
        && oldHorizontalWritingMode != isHorizontalWritingMode())
        RenderBlock::clearPercentHeightDescendantsFrom(this);

    if (hasOverflowClip() && oldStyle && newStyle && oldStyle->effectiveZoom() != newStyle->effectiveZoom() && layer()) {
        if (int left = layer()->scrollableArea()->scrollXOffset()) {
            left = (left / oldStyle->effectiveZoom()) * newStyle->effectiveZoom();
            layer()->scrollableArea()->scrollToXOffset(left);
        }
        if (int top = layer()->scrollableArea()->scrollYOffset()) {
            top = (top / oldStyle->effectiveZoom()) * newStyle->effectiveZoom();
            layer()->scrollableArea()->scrollToYOffset(top);
        }
     }
 
    if (diff.needsRepaint()) {
         RenderObject* parentToInvalidate = parent();
         for (unsigned i = 0; i < backgroundObscurationTestMaxDepth && parentToInvalidate; ++i) {
             parentToInvalidate->invalidateBackgroundObscurationStatus();
            parentToInvalidate = parentToInvalidate->parent();
        }
    }

    if (isDocumentElement() || isBody())
        document().view()->recalculateScrollbarOverlayStyle();

    updateShapeOutsideInfoAfterStyleChange(*style(), oldStyle);
    updateGridPositionAfterStyleChange(oldStyle);
}
