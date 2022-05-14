 double AffineTransform::yScale() const
 {
    return sqrt(m_transform[2] * m_transform[2] + m_transform[3] * m_transform[3]);
 }
