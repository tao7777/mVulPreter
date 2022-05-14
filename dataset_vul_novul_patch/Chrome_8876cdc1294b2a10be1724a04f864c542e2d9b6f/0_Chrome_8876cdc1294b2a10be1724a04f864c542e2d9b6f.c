void SVGAnimateElement::calculateAnimatedValue(float percentage, unsigned repeatCount, SVGSMILElement* resultElement)
 {
     ASSERT(resultElement);
     SVGElement* targetElement = this->targetElement();
    if (!targetElement || !isSVGAnimateElement(*resultElement))
         return;
 
     ASSERT(m_animatedPropertyType == determineAnimatedPropertyType(targetElement));

    ASSERT(percentage >= 0 && percentage <= 1);
    ASSERT(m_animatedPropertyType != AnimatedTransformList || hasTagName(SVGNames::animateTransformTag));
    ASSERT(m_animatedPropertyType != AnimatedUnknown);
    ASSERT(m_animator);
    ASSERT(m_animator->type() == m_animatedPropertyType);
    ASSERT(m_fromType);
    ASSERT(m_fromType->type() == m_animatedPropertyType);
    ASSERT(m_toType);

    SVGAnimateElement* resultAnimationElement = toSVGAnimateElement(resultElement);
    ASSERT(resultAnimationElement->m_animatedType);
    ASSERT(resultAnimationElement->m_animatedPropertyType == m_animatedPropertyType);

    if (hasTagName(SVGNames::setTag))
        percentage = 1;

    if (calcMode() == CalcModeDiscrete)
        percentage = percentage < 0.5 ? 0 : 1;

    m_animator->setContextElement(targetElement);

    if (!m_animatedProperties.isEmpty())
        m_animator->animValWillChange(m_animatedProperties);

    SVGAnimatedType* toAtEndOfDurationType = m_toAtEndOfDurationType ? m_toAtEndOfDurationType.get() : m_toType.get();
    m_animator->calculateAnimatedValue(percentage, repeatCount, m_fromType.get(), m_toType.get(), toAtEndOfDurationType, resultAnimationElement->m_animatedType.get());
}
