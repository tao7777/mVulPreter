String AXNodeObject::textFromDescendants(AXObjectSet& visited,
                                         bool recursive) const {
  if (!canHaveChildren() && recursive)
    return String();

  StringBuilder accumulatedText;
  AXObject* previous = nullptr;

  AXObjectVector children;

  HeapVector<Member<AXObject>> ownedChildren;
  computeAriaOwnsChildren(ownedChildren);
  for (AXObject* obj = rawFirstChild(); obj; obj = obj->rawNextSibling()) {
    if (!axObjectCache().isAriaOwned(obj))
      children.push_back(obj);
  }
  for (const auto& ownedChild : ownedChildren)
    children.push_back(ownedChild);

  for (AXObject* child : children) {
    if (equalIgnoringCase(child->getAttribute(aria_hiddenAttr), "true"))
       continue;
 
    if (previous && accumulatedText.length() &&
        !isHTMLSpace(accumulatedText[accumulatedText.length() - 1])) {
      if (!isInSameNonInlineBlockFlow(child->getLayoutObject(),
                                      previous->getLayoutObject()))
        accumulatedText.append(' ');
    }

    String result;
    if (child->isPresentational())
      result = child->textFromDescendants(visited, true);
    else
      result = recursiveTextAlternative(*child, false, visited);
    accumulatedText.append(result);
    previous = child;
  }

  return accumulatedText.toString();
}
