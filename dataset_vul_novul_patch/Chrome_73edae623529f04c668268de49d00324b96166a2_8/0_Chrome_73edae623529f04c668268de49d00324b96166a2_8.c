Node* DOMPatchSupport::patchNode(Node* node, const String& markup, ExceptionCode& ec)
{
    if (node->isDocumentNode() || (node->parentNode() && node->parentNode()->isDocumentNode())) {
        patchDocument(markup);
        return 0;
     }
 
     Node* previousSibling = node->previousSibling();
    // FIXME: This code should use one of createFragment* in markup.h
     RefPtr<DocumentFragment> fragment = DocumentFragment::create(m_document);
     fragment->parseHTML(markup, node->parentElement() ? node->parentElement() : m_document->documentElement());
 
    ContainerNode* parentNode = node->parentNode();
    Vector<OwnPtr<Digest> > oldList;
    for (Node* child = parentNode->firstChild(); child; child = child->nextSibling())
        oldList.append(createDigest(child, 0));

    String markupCopy = markup;
    markupCopy.makeLower();
    Vector<OwnPtr<Digest> > newList;
    for (Node* child = parentNode->firstChild(); child != node; child = child->nextSibling())
        newList.append(createDigest(child, 0));
    for (Node* child = fragment->firstChild(); child; child = child->nextSibling()) {
        if (child->hasTagName(headTag) && !child->firstChild() && markupCopy.find("</head>") == notFound)
            continue; // HTML5 parser inserts empty <head> tag whenever it parses <body>
        if (child->hasTagName(bodyTag) && !child->firstChild() && markupCopy.find("</body>") == notFound)
            continue; // HTML5 parser inserts empty <body> tag whenever it parses </head>
        newList.append(createDigest(child, &m_unusedNodesMap));
    }
    for (Node* child = node->nextSibling(); child; child = child->nextSibling())
        newList.append(createDigest(child, 0));

    if (!innerPatchChildren(parentNode, oldList, newList, ec)) {
        ec = 0;
        if (!m_domEditor->replaceChild(parentNode, fragment.release(), node, ec))
            return 0;
    }
    return previousSibling ? previousSibling->nextSibling() : parentNode->firstChild();
}
