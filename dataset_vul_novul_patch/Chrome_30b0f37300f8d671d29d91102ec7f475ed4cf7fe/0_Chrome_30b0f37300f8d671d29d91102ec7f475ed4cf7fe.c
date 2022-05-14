InvalidationSet* RuleFeatureSet::invalidationSetForSelector(const CSSSelector& selector, InvalidationType type)
{
    if (selector.match() == CSSSelector::Class)
        return &ensureClassInvalidationSet(selector.value(), type);
    if (selector.isAttributeSelector())
        return &ensureAttributeInvalidationSet(selector.attribute().localName(), type);
    if (selector.match() == CSSSelector::Id)
        return &ensureIdInvalidationSet(selector.value(), type);
    if (selector.match() == CSSSelector::PseudoClass) {
        switch (selector.pseudoType()) {
        case CSSSelector::PseudoEmpty:
        case CSSSelector::PseudoLink:
        case CSSSelector::PseudoVisited:
        case CSSSelector::PseudoAnyLink:
        case CSSSelector::PseudoAutofill:
        case CSSSelector::PseudoHover:
        case CSSSelector::PseudoFocus:
        case CSSSelector::PseudoActive:
        case CSSSelector::PseudoChecked:
        case CSSSelector::PseudoEnabled:
        case CSSSelector::PseudoDisabled:
         case CSSSelector::PseudoOptional:
         case CSSSelector::PseudoPlaceholderShown:
         case CSSSelector::PseudoRequired:
        case CSSSelector::PseudoReadOnly:
        case CSSSelector::PseudoReadWrite:
         case CSSSelector::PseudoValid:
         case CSSSelector::PseudoInvalid:
         case CSSSelector::PseudoIndeterminate:
        case CSSSelector::PseudoTarget:
            return &ensurePseudoInvalidationSet(selector.pseudoType(), type);
        default:
            break;
        }
    }
    return nullptr;
}
