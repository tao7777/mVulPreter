void RenderBlock::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
    RenderBox::styleDidChange(diff, oldStyle);

    RenderStyle* newStyle = style();

    if (!isAnonymousBlock()) {
        for (RenderBlock* currCont = blockElementContinuation(); currCont; currCont = currCont->blockElementContinuation()) {
            RenderBoxModelObject* nextCont = currCont->continuation();
            currCont->setContinuation(0);
            currCont->setStyle(newStyle);
            currCont->setContinuation(nextCont);
        }
    }

    if (FastTextAutosizer* textAutosizer = document().fastTextAutosizer())
        textAutosizer->record(this);

    propagateStyleToAnonymousChildren(true);
    invalidateLineHeight();
 
    m_hasBorderOrPaddingLogicalWidthChanged = oldStyle && diff == StyleDifferenceLayout && needsLayout() && borderOrPaddingLogicalWidthChanged(oldStyle, newStyle);
 
    Vector<ImageResource*> images;
    appendImagesFromStyle(images, *newStyle);
    if (images.isEmpty())
        ResourceLoadPriorityOptimizer::resourceLoadPriorityOptimizer()->removeRenderObject(this);
    else
        ResourceLoadPriorityOptimizer::resourceLoadPriorityOptimizer()->addRenderObject(this);
}
