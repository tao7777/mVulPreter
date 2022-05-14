 double AffineTransform::xScale() const
 {
    return sqrt(m_transform[0] * m_transform[0] + m_transform[1] * m_transform[1]);
 }
