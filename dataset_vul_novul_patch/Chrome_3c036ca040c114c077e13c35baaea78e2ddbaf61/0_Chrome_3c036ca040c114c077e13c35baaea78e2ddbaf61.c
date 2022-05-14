 WebTransformationMatrix WebTransformOperations::blend(const WebTransformOperations& from, double progress) const
 {
     WebTransformationMatrix toReturn;
    blendInternal(from, progress, toReturn);
     return toReturn;
 }
