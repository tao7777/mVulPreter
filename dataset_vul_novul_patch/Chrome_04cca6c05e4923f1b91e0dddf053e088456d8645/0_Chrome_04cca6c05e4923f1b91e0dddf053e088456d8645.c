bool InlineFlowBox::nodeAtPoint(const HitTestRequest& request, HitTestResult& result, int x, int y, int tx, int ty)
{
    IntRect overflowRect(visualOverflowRect());
    flipForWritingMode(overflowRect);
    overflowRect.move(tx, ty);
    if (!overflowRect.intersects(result.rectForPoint(x, y)))
        return false;

    for (InlineBox* curr = lastChild(); curr; curr = curr->prevOnLine()) {
        if ((curr->renderer()->isText() || !curr->boxModelObject()->hasSelfPaintingLayer()) && curr->nodeAtPoint(request, result, x, y, tx, ty)) {
            renderer()->updateHitTestResult(result, IntPoint(x - tx, y - ty));
            return true;
         }
     }
 
    // Now check ourselves. Pixel snap hit testing.
    IntRect frameRect = roundedFrameRect();
    int minX = frameRect.x();
    int minY = frameRect.y();
    int width = frameRect.width();
    int height = frameRect.height();

    // Constrain our hit testing to the line top and bottom if necessary.
    bool noQuirksMode = renderer()->document()->inNoQuirksMode();
    if (!hasTextChildren() && !noQuirksMode) {
        RootInlineBox* rootBox = root();
        int& top = isHorizontal() ? minY : minX;
        int& logicalHeight = isHorizontal() ? height : width;
        int bottom = min(rootBox->lineBottom(), top + logicalHeight);
        top = max(rootBox->lineTop(), top);
        logicalHeight = bottom - top;
    }
    
    // Move x/y to our coordinates.
    IntRect rect(minX, minY, width, height);
    flipForWritingMode(rect);
    rect.move(tx, ty);

     if (visibleToHitTesting() && rect.intersects(result.rectForPoint(x, y))) {
         renderer()->updateHitTestResult(result, flipForWritingMode(IntPoint(x - tx, y - ty))); // Don't add in m_x or m_y here, we want coords in the containing block's space.
         if (!result.addNodeToRectBasedTestResult(renderer()->node(), x, y, rect))
            return true;
    }
    
    return false;
}
