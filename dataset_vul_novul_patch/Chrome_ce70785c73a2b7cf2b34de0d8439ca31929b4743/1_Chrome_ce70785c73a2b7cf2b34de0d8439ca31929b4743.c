LayoutUnit LayoutBlockFlow::adjustBlockChildForPagination(LayoutUnit logicalTop, LayoutBox& child, bool atBeforeSideOfBlock)
{
    LayoutBlockFlow* childBlockFlow = child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child) : 0;

    LayoutUnit newLogicalTop = applyBeforeBreak(child, logicalTop);

    LayoutUnit logicalTopBeforeUnsplittableAdjustment = newLogicalTop;
    LayoutUnit logicalTopAfterUnsplittableAdjustment = adjustForUnsplittableChild(child, newLogicalTop);

    LayoutUnit paginationStrut = 0;
    LayoutUnit unsplittableAdjustmentDelta = logicalTopAfterUnsplittableAdjustment - logicalTopBeforeUnsplittableAdjustment;
    LayoutUnit childLogicalHeight = child.logicalHeight();
    if (unsplittableAdjustmentDelta) {
        setPageBreak(newLogicalTop, childLogicalHeight - unsplittableAdjustmentDelta);
        paginationStrut = unsplittableAdjustmentDelta;
    } else if (childBlockFlow && childBlockFlow->paginationStrut()) {
        paginationStrut = childBlockFlow->paginationStrut();
    }

     if (paginationStrut) {
        if (atBeforeSideOfBlock && logicalTop == newLogicalTop && !isOutOfFlowPositioned() && !isTableCell()) {
            paginationStrut += logicalTop;
            if (isFloating())
                paginationStrut += marginBefore(); // Floats' margins do not collapse with page or column boundaries.
            setPaginationStrut(paginationStrut);
            if (childBlockFlow)
                childBlockFlow->setPaginationStrut(0);
        } else {
            newLogicalTop += paginationStrut;
        }
    }

    if (!unsplittableAdjustmentDelta) {
        if (LayoutUnit pageLogicalHeight = pageLogicalHeightForOffset(newLogicalTop)) {
            LayoutUnit remainingLogicalHeight = pageRemainingLogicalHeightForOffset(newLogicalTop, AssociateWithLatterPage);
            LayoutUnit spaceShortage = childLogicalHeight - remainingLogicalHeight;
            if (spaceShortage > 0) {
                LayoutUnit spaceShortageInLastColumn = intMod(spaceShortage, pageLogicalHeight);
                setPageBreak(newLogicalTop, spaceShortageInLastColumn ? spaceShortageInLastColumn : spaceShortage);
            } else if (remainingLogicalHeight == pageLogicalHeight && offsetFromLogicalTopOfFirstPage() + child.logicalTop()) {
                setPageBreak(newLogicalTop, childLogicalHeight);
            }
        }
    }

    setLogicalHeight(logicalHeight() + (newLogicalTop - logicalTop));

    return newLogicalTop;
}
