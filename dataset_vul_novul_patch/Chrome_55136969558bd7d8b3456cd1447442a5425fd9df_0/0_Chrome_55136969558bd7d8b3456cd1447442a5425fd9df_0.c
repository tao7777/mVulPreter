 void DisplayItemList::processNewItem(DisplayItem& displayItem)
{
    ASSERT(!m_constructionDisabled);
    ASSERT(!skippingCache() || !displayItem.isCached());

    if (displayItem.isCached())
        ++m_numCachedItems;

#if ENABLE(ASSERT)
    if (m_newDisplayItems.size() >= 2 && displayItem.isEnd()) {
        const auto& beginDisplayItem = m_newDisplayItems[m_newDisplayItems.size() - 2];
        if (beginDisplayItem.isBegin() && !beginDisplayItem.isSubsequence() && !beginDisplayItem.drawsContent())
            ASSERT(!displayItem.isEndAndPairedWith(beginDisplayItem.type()));
    }
#endif

    if (!m_scopeStack.isEmpty())
        displayItem.setScope(m_scopeStack.last());

#if ENABLE(ASSERT)
    size_t index = findMatchingItemFromIndex(displayItem.nonCachedId(), m_newDisplayItemIndicesByClient, m_newDisplayItems);
    if (index != kNotFound) {
#ifndef NDEBUG
        showDebugData();
        WTFLogAlways("DisplayItem %s has duplicated id with previous %s (index=%d)\n",
            displayItem.asDebugString().utf8().data(), m_newDisplayItems[index].asDebugString().utf8().data(), static_cast<int>(index));
#endif
        ASSERT_NOT_REACHED();
    }
    addItemToIndexIfNeeded(displayItem, m_newDisplayItems.size() - 1, m_newDisplayItemIndicesByClient);
#endif // ENABLE(ASSERT)
 
     if (skippingCache())
         displayItem.setSkippedCache();

    if (RuntimeEnabledFeatures::slimmingPaintV2Enabled())
        m_newPaintChunks.incrementDisplayItemIndex();
}

void DisplayItemList::updateCurrentPaintProperties(const PaintProperties& newPaintProperties)
{
    m_newPaintChunks.updateCurrentPaintProperties(newPaintProperties);
 }
