bool SVGFEColorMatrixElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
     FEColorMatrix* colorMatrix = static_cast<FEColorMatrix*>(effect);
     if (attrName == SVGNames::typeAttr)
         return colorMatrix->setType(m_type->currentValue()->enumValue());
    if (attrName == SVGNames::valuesAttr) {
        Vector<float> values = m_values->currentValue()->toFloatVector();
        if (values.size() == 20)
            return colorMatrix->setValues(m_values->currentValue()->toFloatVector());
        return false;
    }
 
     ASSERT_NOT_REACHED();
     return false;
}
