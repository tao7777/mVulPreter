static inline void removeElementPreservingChildren(PassRefPtr<DocumentFragment> fragment, HTMLElement* element)
{
    ExceptionCode ignoredExceptionCode;
    RefPtr<Node> nextChild;
    for (RefPtr<Node> child = element->firstChild(); child; child = nextChild) {
        nextChild = child->nextSibling();
        element->removeChild(child.get(), ignoredExceptionCode);
        ASSERT(!ignoredExceptionCode);
        fragment->insertBefore(child, element, ignoredExceptionCode);
        ASSERT(!ignoredExceptionCode);
    }
    fragment->removeChild(element, ignoredExceptionCode);
    ASSERT(!ignoredExceptionCode);
}
