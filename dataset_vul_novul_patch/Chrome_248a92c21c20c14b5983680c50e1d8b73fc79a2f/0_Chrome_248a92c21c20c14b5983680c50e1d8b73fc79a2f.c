static inline void constructBidiRunsForSegment(InlineBidiResolver& topResolver, BidiRunList<BidiRun>& bidiRuns, const InlineIterator& endOfRuns, VisualDirectionOverride override, bool previousLineBrokeCleanly)
{
    ASSERT(&topResolver.runs() == &bidiRuns);
    ASSERT(topResolver.position() != endOfRuns);
    RenderObject* currentRoot = topResolver.position().root();
    topResolver.createBidiRunsForLine(endOfRuns, override, previousLineBrokeCleanly);

    while (!topResolver.isolatedRuns().isEmpty()) {
        BidiRun* isolatedRun = topResolver.isolatedRuns().last();
        topResolver.isolatedRuns().removeLast();

        RenderObject* startObj = isolatedRun->object();

        RenderInline* isolatedInline = toRenderInline(highestContainingIsolateWithinRoot(startObj, currentRoot));
        ASSERT(isolatedInline);
 
         InlineBidiResolver isolatedResolver;
         EUnicodeBidi unicodeBidi = isolatedInline->style()->unicodeBidi();
        TextDirection direction = isolatedInline->style()->direction();
        if (unicodeBidi == Plaintext)
            direction = determinePlaintextDirectionality(isolatedInline, startObj);
        else {
            ASSERT(unicodeBidi == Isolate || unicodeBidi == IsolateOverride);
            direction = isolatedInline->style()->direction();
        }
        isolatedResolver.setStatus(statusWithDirection(direction, isOverride(unicodeBidi)));

        setupResolverToResumeInIsolate(isolatedResolver, isolatedInline, startObj);

        InlineIterator iter = InlineIterator(isolatedInline, startObj, isolatedRun->m_start);
        isolatedResolver.setPositionIgnoringNestedIsolates(iter);

        isolatedResolver.createBidiRunsForLine(endOfRuns, NoVisualOverride, previousLineBrokeCleanly);
        if (isolatedResolver.runs().runCount())
            bidiRuns.replaceRunWithRuns(isolatedRun, isolatedResolver.runs());

        if (!isolatedResolver.isolatedRuns().isEmpty()) {
            topResolver.isolatedRuns().append(isolatedResolver.isolatedRuns());
            isolatedResolver.isolatedRuns().clear();
            currentRoot = isolatedInline;
        }
    }
}
