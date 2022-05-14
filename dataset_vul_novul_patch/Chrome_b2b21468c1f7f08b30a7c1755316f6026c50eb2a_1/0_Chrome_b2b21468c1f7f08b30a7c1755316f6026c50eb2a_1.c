void RenderBlock::styleWillChange(StyleDifference diff, const RenderStyle& newStyle)
{
    RenderStyle* oldStyle = style();
 
     setReplaced(newStyle.isDisplayInlineType());
 
    if (oldStyle && parent() && diff.needsFullLayout() && oldStyle->position() != newStyle.position()) {
         if (newStyle.position() == StaticPosition)
            removePositionedObjects(0, NewContainingBlock);
        else if (oldStyle->position() == StaticPosition) {
            RenderObject* cb = parent();
            while (cb && (cb->style()->position() == StaticPosition || (cb->isInline() && !cb->isReplaced())) && !cb->isRenderView()) {
                if (cb->style()->position() == RelativePosition && cb->isInline() && !cb->isReplaced()) {
                    cb = cb->containingBlock();
                    break;
                }
                cb = cb->parent();
            }

            if (cb->isRenderBlock())
                toRenderBlock(cb)->removePositionedObjects(this, NewContainingBlock);
        }
    }

    RenderBox::styleWillChange(diff, newStyle);
}
