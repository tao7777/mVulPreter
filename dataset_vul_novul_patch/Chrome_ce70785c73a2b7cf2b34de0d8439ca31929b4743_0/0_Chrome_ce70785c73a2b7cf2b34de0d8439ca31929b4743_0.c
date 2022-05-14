static inline bool shouldSetStrutOnBlock(const LayoutBlockFlow& block, const RootInlineBox& lineBox, LayoutUnit lineLogicalOffset, int lineIndex, LayoutUnit remainingLogicalHeight)
{
    bool wantsStrutOnBlock = false;
    if (!block.style()->hasAutoOrphans() && block.style()->orphans() >= lineIndex) {
        wantsStrutOnBlock = true;
    } else if (lineBox == block.firstRootBox() && lineLogicalOffset == block.borderAndPaddingBefore()) {
        LayoutUnit lineHeight = lineBox.lineBottomWithLeading() - lineBox.lineTopWithLeading();
        LayoutUnit totalLogicalHeight = lineHeight + std::max<LayoutUnit>(0, lineLogicalOffset);
        LayoutUnit pageLogicalHeightAtNewOffset = block.pageLogicalHeightForOffset(lineLogicalOffset + remainingLogicalHeight);
         if (totalLogicalHeight < pageLogicalHeightAtNewOffset)
             wantsStrutOnBlock = true;
     }
    return wantsStrutOnBlock && block.allowsPaginationStrut();
 }
