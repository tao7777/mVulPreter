void RenderBox::styleWillChange(StyleDifference diff, const RenderStyle& newStyle)
{
    RenderStyle* oldStyle = style();
     if (oldStyle) {
        if ((diff.needsRepaint() || diff.needsLayout()) && node()
            && (isHTMLHtmlElement(*node()) || isHTMLBodyElement(*node()))) {
             view()->repaint();
 
             if (oldStyle->hasEntirelyFixedBackground() != newStyle.hasEntirelyFixedBackground())
                view()->compositor()->setNeedsUpdateFixedBackground();
        }
 
        if (diff.needsFullLayout() && parent() && oldStyle->position() != newStyle.position()) {
             markContainingBlocksForLayout();
             if (oldStyle->position() == StaticPosition)
                 repaint();
            else if (newStyle.hasOutOfFlowPosition())
                parent()->setChildNeedsLayout();
            if (isFloating() && !isOutOfFlowPositioned() && newStyle.hasOutOfFlowPosition())
                removeFloatingOrPositionedChildFromBlockLists();
        }
    } else if (isBody())
        view()->repaint();

    RenderBoxModelObject::styleWillChange(diff, newStyle);
}
