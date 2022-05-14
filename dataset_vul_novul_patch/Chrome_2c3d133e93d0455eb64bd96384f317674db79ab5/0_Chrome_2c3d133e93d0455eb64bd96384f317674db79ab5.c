IntRect InlineTextBox::selectionRect(int tx, int ty, int startPos, int endPos)
{
    int sPos = max(startPos - m_start, 0);
    int ePos = min(endPos - m_start, (int)m_len);
    
    if (sPos > ePos)
        return IntRect();

    RenderText* textObj = textRenderer();
    int selTop = selectionTop();
    int selHeight = selectionHeight();
    RenderStyle* styleToUse = textObj->style(m_firstLine);
    const Font& f = styleToUse->font();

    const UChar* characters = textObj->text()->characters() + m_start;
    int len = m_len;
    BufferForAppendingHyphen charactersWithHyphen;
    if (ePos == len && hasHyphen()) {
        adjustCharactersAndLengthForHyphen(charactersWithHyphen, styleToUse, characters, len);
        ePos = len;
     }
 
     IntRect r = enclosingIntRect(f.selectionRectForText(TextRun(characters, len, textObj->allowTabs(), textPos(), m_toAdd, !isLeftToRightDirection(), m_dirOverride),
                                                        IntPoint(), selHeight, sPos, ePos));
                                                        
    int logicalWidth = r.width();
    if (r.x() > m_logicalWidth)
        logicalWidth  = 0;
    else if (r.right() > m_logicalWidth)
        logicalWidth = m_logicalWidth - r.x();
    
    IntPoint topPoint = m_isVertical ? IntPoint(tx + selTop, ty + m_y + r.x()) : IntPoint(tx + m_x + r.x(), ty + selTop);
    int width = m_isVertical ? selHeight : logicalWidth;
    int height = m_isVertical ? logicalWidth : selHeight;
    
    return IntRect(topPoint, IntSize(width, height));
 }
