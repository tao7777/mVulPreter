void DisplayItemList::removeLastDisplayItem()
{
    if (m_newDisplayItems.isEmpty())
        return;

#if ENABLE(ASSERT)
    DisplayItemIndicesByClientMap::iterator it = m_newDisplayItemIndicesByClient.find(m_newDisplayItems.last().client());
    if (it != m_newDisplayItemIndicesByClient.end()) {
        Vector<size_t>& indices = it->value;
        if (!indices.isEmpty() && indices.last() == (m_newDisplayItems.size() - 1))
            indices.removeLast();
     }
 #endif
     m_newDisplayItems.removeLast();
 }
