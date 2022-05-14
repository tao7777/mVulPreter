bool LayoutSVGTransformableContainer::calculateLocalTransform()
void LayoutSVGTransformableContainer::setNeedsTransformUpdate()
{
    setMayNeedPaintInvalidationSubtree();
    m_needsTransformUpdate = true;
}

static std::pair<double, double> scaleReference(const AffineTransform& transform)
{
    return std::make_pair(transform.xScaleSquared(), transform.yScaleSquared());
}

LayoutSVGContainer::TransformChange LayoutSVGTransformableContainer::calculateLocalTransform()
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
        // TODO(fs): Signal this on style update instead. (Since these are
        // suppose to be presentation attributes now, this does feel a bit
        // broken...)
         if (translation != m_additionalTranslation)
            setNeedsTransformUpdate();
         m_additionalTranslation = translation;
     }
 
     if (!m_needsTransformUpdate)
        return TransformChange::None;
 
    std::pair<double, double> oldScale = scaleReference(m_localTransform);
     m_localTransform = element->calculateAnimatedLocalTransform();
     m_localTransform.translate(m_additionalTranslation.width(), m_additionalTranslation.height());
     m_needsTransformUpdate = false;
    return scaleReference(m_localTransform) != oldScale
        ? TransformChange::Full : TransformChange::ScaleInvariant;
 }
