 double AffineTransform::xScale() const
 {
    return sqrt(xScaleSquared());
}

double AffineTransform::yScaleSquared() const
{
    return m_transform[2] * m_transform[2] + m_transform[3] * m_transform[3];
 }
