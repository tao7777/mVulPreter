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
                                                        IntPoint(tx + m_x, ty + selTop), selHeight, sPos, ePos));
    if (r.x() > tx + m_x + m_logicalWidth)
        r.setWidth(0);
    else if (r.right() - 1 > tx + m_x + m_logicalWidth)
        r.setWidth(tx + m_x + m_logicalWidth - r.x());
    return r;
 }
