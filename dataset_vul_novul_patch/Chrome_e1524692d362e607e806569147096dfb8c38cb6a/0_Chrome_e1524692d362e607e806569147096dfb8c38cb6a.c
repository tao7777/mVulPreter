void ApplyBlockElementCommand::formatSelection(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    Position start = startOfSelection.deepEquivalent().downstream();
    if (isAtUnsplittableElement(start)) {
        RefPtr<Element> blockquote = createBlockElement();
        insertNodeAt(blockquote, start);
        RefPtr<Element> placeholder = createBreakElement(document());
        appendNode(placeholder, blockquote);
        setEndingSelection(VisibleSelection(positionBeforeNode(placeholder.get()), DOWNSTREAM, endingSelection().isDirectional()));
        return;
    }

    RefPtr<Element> blockquoteForNextIndent;
    VisiblePosition endOfCurrentParagraph = endOfParagraph(startOfSelection);
    VisiblePosition endAfterSelection = endOfParagraph(endOfParagraph(endOfSelection).next());
    m_endOfLastParagraph = endOfParagraph(endOfSelection).deepEquivalent();

    bool atEnd = false;
    Position end;
    while (endOfCurrentParagraph != endAfterSelection && !atEnd) {
        if (endOfCurrentParagraph.deepEquivalent() == m_endOfLastParagraph)
            atEnd = true;

        rangeForParagraphSplittingTextNodesIfNeeded(endOfCurrentParagraph, start, end);
        endOfCurrentParagraph = end;

        Position afterEnd = end.next();
        Node* enclosingCell = enclosingNodeOfType(start, &isTableCell);
        VisiblePosition endOfNextParagraph = endOfNextParagrahSplittingTextNodesIfNeeded(endOfCurrentParagraph, start, end);

        formatRange(start, end, m_endOfLastParagraph, blockquoteForNextIndent);

        if (enclosingCell && enclosingCell != enclosingNodeOfType(endOfNextParagraph.deepEquivalent(), &isTableCell))
            blockquoteForNextIndent = 0;

         if (endAfterSelection.isNotNull() && !endAfterSelection.deepEquivalent().inDocument())
             break;
        // If somehow, e.g. mutation event handler, we did, return to prevent crashes.
        if (endOfNextParagraph.isNotNull() && !endOfNextParagraph.deepEquivalent().inDocument())
             return;
         endOfCurrentParagraph = endOfNextParagraph;
     }
 }
