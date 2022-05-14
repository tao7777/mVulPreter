void RenderBlockFlow::styleWillChange(StyleDifference diff, const RenderStyle& newStyle)
 {
     RenderStyle* oldStyle = style();
     s_canPropagateFloatIntoSibling = oldStyle ? !isFloatingOrOutOfFlowPositioned() && !avoidsFloats() : false;
    if (oldStyle && parent() && diff.needsFullLayout() && oldStyle->position() != newStyle.position()
         && containsFloats() && !isFloating() && !isOutOfFlowPositioned() && newStyle.hasOutOfFlowPosition())
             markAllDescendantsWithFloatsForLayout();
 
    RenderBlock::styleWillChange(diff, newStyle);
}
