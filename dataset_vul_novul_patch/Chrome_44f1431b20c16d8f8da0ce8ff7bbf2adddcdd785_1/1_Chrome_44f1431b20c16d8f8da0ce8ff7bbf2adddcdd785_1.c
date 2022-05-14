bool LayoutSVGTransformableContainer::calculateLocalTransform()
 {
     SVGGraphicsElement* element = toSVGGraphicsElement(this->element());
     ASSERT(element);

    SVGUseElement* useElement = nullptr;
    if (isSVGUseElement(*element)) {
        useElement = toSVGUseElement(element);
    } else if (isSVGGElement(*element) && toSVGGElement(element)->inUseShadowTree()) {
        SVGElement* correspondingElement = element->correspondingElement();
        if (isSVGUseElement(correspondingElement))
            useElement = toSVGUseElement(correspondingElement);
    }

    if (useElement) {
        SVGLengthContext lengthContext(useElement);
         FloatSize translation(
             useElement->x()->currentValue()->value(lengthContext),
             useElement->y()->currentValue()->value(lengthContext));
         if (translation != m_additionalTranslation)
            m_needsTransformUpdate = true;
         m_additionalTranslation = translation;
     }
 
     if (!m_needsTransformUpdate)
        return false;
 
     m_localTransform = element->calculateAnimatedLocalTransform();
     m_localTransform.translate(m_additionalTranslation.width(), m_additionalTranslation.height());
     m_needsTransformUpdate = false;
    return true;
 }
