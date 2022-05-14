bool SimplifiedBackwardsTextIterator::handleTextNode()
{
    m_lastTextNode = m_node;

    int startOffset;
    int offsetInNode;
    RenderText* renderer = handleFirstLetter(startOffset, offsetInNode);
    if (!renderer)
        return true;

    String text = renderer->text();
    if (!renderer->firstTextBox() && text.length() > 0)
        return true;

    m_positionEndOffset = m_offset;
    m_offset = startOffset + offsetInNode;
    m_positionNode = m_node;
    m_positionStartOffset = m_offset;

    ASSERT(0 <= m_positionStartOffset - offsetInNode && m_positionStartOffset - offsetInNode <= static_cast<int>(text.length()));
    ASSERT(1 <= m_positionEndOffset - offsetInNode && m_positionEndOffset - offsetInNode <= static_cast<int>(text.length()));
    ASSERT(m_positionStartOffset <= m_positionEndOffset);

     m_textLength = m_positionEndOffset - m_positionStartOffset;
     m_textCharacters = text.characters() + (m_positionStartOffset - offsetInNode);
     ASSERT(m_textCharacters >= text.characters());
    RELEASE_ASSERT(m_textCharacters + m_textLength <= text.characters() + static_cast<int>(text.length()));
 
     m_lastCharacter = text[m_positionEndOffset - 1];
 
    return !m_shouldHandleFirstLetter;
}
