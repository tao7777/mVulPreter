void DisplayItemList::commitNewDisplayItems(GraphicsLayer* graphicsLayer)
{
    TRACE_EVENT2("blink,benchmark", "DisplayItemList::commitNewDisplayItems", "current_display_list_size", (int)m_currentDisplayItems.size(),
        "num_non_cached_new_items", (int)m_newDisplayItems.size() - m_numCachedItems);

    if (RuntimeEnabledFeatures::slimmingPaintSynchronizedPaintingEnabled()) {
        for (const auto& invalidation : m_invalidations)
            graphicsLayer->setNeedsDisplayInRect(invalidation.rect, invalidation.invalidationReason);
        m_invalidations.clear();
    }

    ASSERT(m_scopeStack.isEmpty());
    m_scopeStack.clear();
    m_nextScope = 1;
    ASSERT(!skippingCache());
#if ENABLE(ASSERT)
    m_newDisplayItemIndicesByClient.clear();
#endif

    if (m_currentDisplayItems.isEmpty()) {
#if ENABLE(ASSERT)
        for (const auto& item : m_newDisplayItems)
             ASSERT(!item.isCached());
 #endif
         m_currentDisplayItems.swap(m_newDisplayItems);
         m_validlyCachedClientsDirty = true;
         m_numCachedItems = 0;
         return;
    }

    updateValidlyCachedClientsIfNeeded();

    OutOfOrderIndexContext outOfOrderIndexContext(m_currentDisplayItems.begin());
 
     DisplayItems updatedList(std::max(m_currentDisplayItems.usedCapacityInBytes(), m_newDisplayItems.usedCapacityInBytes()));
     DisplayItems::iterator currentIt = m_currentDisplayItems.begin();
     DisplayItems::iterator currentEnd = m_currentDisplayItems.end();
     for (DisplayItems::iterator newIt = m_newDisplayItems.begin(); newIt != m_newDisplayItems.end(); ++newIt) {
        const DisplayItem& newDisplayItem = *newIt;
        const DisplayItem::Id newDisplayItemId = newDisplayItem.nonCachedId();
        bool newDisplayItemHasCachedType = newDisplayItem.type() != newDisplayItemId.type;

        bool isSynchronized = currentIt != currentEnd && newDisplayItemId.matches(*currentIt);

        if (newDisplayItemHasCachedType) {
            ASSERT(newDisplayItem.isCached());
            ASSERT(clientCacheIsValid(newDisplayItem.client()) || (RuntimeEnabledFeatures::slimmingPaintOffsetCachingEnabled() && !paintOffsetWasInvalidated(newDisplayItem.client())));
            if (!isSynchronized) {
                currentIt = findOutOfOrderCachedItem(newDisplayItemId, outOfOrderIndexContext);

                if (currentIt == currentEnd) {
#ifndef NDEBUG
                    showDebugData();
                    WTFLogAlways("%s not found in m_currentDisplayItems\n", newDisplayItem.asDebugString().utf8().data());
#endif
                    ASSERT_NOT_REACHED();
                    continue;
                }
            }
#if ENABLE(ASSERT)
            if (RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled()) {
                DisplayItems::iterator temp = currentIt;
                checkUnderInvalidation(newIt, temp);
            }
#endif
            if (newDisplayItem.isCachedDrawing()) {
                updatedList.appendByMoving(*currentIt);
                ++currentIt;
            } else {
                ASSERT(newDisplayItem.isCachedSubsequence());
                copyCachedSubsequence(currentIt, updatedList);
                ASSERT(updatedList.last().isEndSubsequence());
            }
        } else {
            ASSERT(!newDisplayItem.isDrawing()
                || newDisplayItem.skippedCache()
                || !clientCacheIsValid(newDisplayItem.client())
                || (RuntimeEnabledFeatures::slimmingPaintOffsetCachingEnabled() && paintOffsetWasInvalidated(newDisplayItem.client())));

            updatedList.appendByMoving(*newIt);

            if (isSynchronized)
                ++currentIt;
        }
        if (currentIt - outOfOrderIndexContext.nextItemToIndex > 0)
            outOfOrderIndexContext.nextItemToIndex = currentIt;
    }

#if ENABLE(ASSERT)
    if (RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled())
         checkNoRemainingCachedDisplayItems();
 #endif // ENABLE(ASSERT)
 
     m_newDisplayItems.clear();
     m_validlyCachedClientsDirty = true;
     m_currentDisplayItems.swap(updatedList);
    m_numCachedItems = 0;

#if ENABLE(ASSERT)
    m_clientsWithPaintOffsetInvalidations.clear();
#endif
}
