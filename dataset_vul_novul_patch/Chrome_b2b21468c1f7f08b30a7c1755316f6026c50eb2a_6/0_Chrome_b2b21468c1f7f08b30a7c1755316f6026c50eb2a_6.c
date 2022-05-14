void RenderFlexibleBox::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
 {
     RenderBlock::styleDidChange(diff, oldStyle);
 
    if (oldStyle && oldStyle->alignItems() == ItemPositionStretch && diff.needsFullLayout()) {
         for (RenderBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
            ItemPosition previousAlignment = resolveAlignment(oldStyle, child->style());
            if (previousAlignment == ItemPositionStretch && previousAlignment != resolveAlignment(style(), child->style()))
                child->setChildNeedsLayout(MarkOnlyThis);
        }
    }
}
