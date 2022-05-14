bool SVGFEColorMatrixElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
     FEColorMatrix* colorMatrix = static_cast<FEColorMatrix*>(effect);
     if (attrName == SVGNames::typeAttr)
         return colorMatrix->setType(m_type->currentValue()->enumValue());
    if (attrName == SVGNames::valuesAttr)
        return colorMatrix->setValues(m_values->currentValue()->toFloatVector());
 
     ASSERT_NOT_REACHED();
     return false;
}
