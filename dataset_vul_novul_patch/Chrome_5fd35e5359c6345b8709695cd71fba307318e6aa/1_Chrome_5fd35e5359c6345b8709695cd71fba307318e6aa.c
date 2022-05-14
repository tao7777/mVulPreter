LayoutUnit RenderBox::availableLogicalHeightUsing(const Length& h) const
{
    if (h.isFixed())
        return computeContentBoxLogicalHeight(h.value());

    if (isRenderView())
        return isHorizontalWritingMode() ? toRenderView(this)->frameView()->visibleHeight() : toRenderView(this)->frameView()->visibleWidth();

     if (isTableCell() && (h.isAuto() || h.isPercent()))
         return overrideHeight() - borderAndPaddingLogicalWidth();
 
    if (h.isPercent())
       return computeContentBoxLogicalHeight(h.calcValue(containingBlock()->availableLogicalHeight()));
 
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
