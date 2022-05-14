 Node::InsertionNotificationRequest HTMLBodyElement::insertedInto(ContainerNode* insertionPoint)
 {
     HTMLElement::insertedInto(insertionPoint);
    if (insertionPoint->inDocument()) {
        Element* ownerElement = document().ownerElement();
        if (isHTMLFrameElementBase(ownerElement)) {
            HTMLFrameElementBase& ownerFrameElement = toHTMLFrameElementBase(*ownerElement);
            int marginWidth = ownerFrameElement.marginWidth();
            if (marginWidth != -1)
                setIntegralAttribute(marginwidthAttr, marginWidth);
            int marginHeight = ownerFrameElement.marginHeight();
            if (marginHeight != -1)
                setIntegralAttribute(marginheightAttr, marginHeight);
        }
    }
    return InsertionDone;
 }
