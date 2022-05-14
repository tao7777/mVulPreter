PassRefPtr<Attr> Element::setAttributeNode(Attr* attrNode, ExceptionCode& ec)
{
    if (!attrNode) {
        ec = TYPE_MISMATCH_ERR;
        return 0;
    }

    RefPtr<Attr> oldAttrNode = attrIfExists(attrNode->qualifiedName());
    if (oldAttrNode.get() == attrNode)
        return attrNode; // This Attr is already attached to the element.

    if (attrNode->ownerElement()) {
        ec = INUSE_ATTRIBUTE_ERR;
        return 0;
    }

    synchronizeAllAttributes();
    UniqueElementData* elementData = ensureUniqueElementData();

    size_t index = elementData->getAttributeItemIndex(attrNode->qualifiedName());
    if (index != notFound) {
        if (oldAttrNode)
            detachAttrNodeFromElementWithValue(oldAttrNode.get(), elementData->attributeItem(index)->value());
        else
            oldAttrNode = Attr::create(document(), attrNode->qualifiedName(), elementData->attributeItem(index)->value());
    }

     setAttributeInternal(index, attrNode->qualifiedName(), attrNode->value(), NotInSynchronizationOfLazyAttribute);
 
     attrNode->attachToElement(this);
    treeScope()->adoptIfNeeded(attrNode);
     ensureAttrNodeListForElement(this)->append(attrNode);
 
     return oldAttrNode.release();
}
