 WebTransformationMatrix WebTransformOperations::blend(const WebTransformOperations& from, double progress) const
 {
     WebTransformationMatrix toReturn;
    bool fromIdentity = from.isIdentity();
    bool toIdentity = isIdentity();
    if (fromIdentity && toIdentity)
        return toReturn;
    if (matchesTypes(from)) {
        size_t numOperations = max(fromIdentity ? 0 : from.m_private->operations.size(),
                                   toIdentity ? 0 : m_private->operations.size());
        for (size_t i = 0; i < numOperations; ++i) {
            WebTransformationMatrix blended = blendTransformOperations(
                fromIdentity ? 0 : &from.m_private->operations[i],
                toIdentity ? 0 : &m_private->operations[i],
                progress);
            toReturn.multiply(blended);
        }
    } else {
        toReturn = apply();
        WebTransformationMatrix fromTransform = from.apply();
        toReturn.blend(fromTransform, progress);
    }
     return toReturn;
 }
