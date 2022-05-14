LayoutUnit RenderBox::availableLogicalHeightUsing(const Length& h) const
{
    if (h.isFixed())
        return computeContentBoxLogicalHeight(h.value());

    if (isRenderView())
        return isHorizontalWritingMode() ? toRenderView(this)->frameView()->visibleHeight() : toRenderView(this)->frameView()->visibleWidth();

     if (isTableCell() && (h.isAuto() || h.isPercent()))
         return overrideHeight() - borderAndPaddingLogicalWidth();
 
    if (h.isPercent()) {
        LayoutUnit availableHeight;
        // https://bugs.webkit.org/show_bug.cgi?id=64046
        // For absolutely positioned elements whose containing block is based on a block-level element,
        // the percentage is calculated with respect to the height of the padding box of that element
        if (isPositioned())
            availableHeight = containingBlockLogicalHeightForPositioned(containingBlock());
        else
            availableHeight = containingBlock()->availableLogicalHeight();
        return computeContentBoxLogicalHeight(h.calcValue(availableHeight));
    }
 
    if (isRenderBlock() && isPositioned() && style()->height().isAuto() && !(style()->top().isAuto() || style()->bottom().isAuto())) {
        RenderBlock* block = const_cast<RenderBlock*>(toRenderBlock(this));
        LayoutUnit oldHeight = block->logicalHeight();
        block->computeLogicalHeight();
        LayoutUnit newHeight = block->computeContentBoxLogicalHeight(block->contentLogicalHeight());
        block->setLogicalHeight(oldHeight);
        return computeContentBoxLogicalHeight(newHeight);
    }

    return containingBlock()->availableLogicalHeight();
}
