static WebTransformationMatrix blendTransformOperations(const WebTransformOperation* from, const WebTransformOperation* to, double progress)
 {
    WebTransformationMatrix toReturn;
     if (isIdentity(from) && isIdentity(to))
        return toReturn;
 
     WebTransformOperation::Type interpolationType = WebTransformOperation::WebTransformOperationIdentity;
     if (isIdentity(to))
        interpolationType = from->type;
    else
        interpolationType = to->type;

    switch (interpolationType) {
    case WebTransformOperation::WebTransformOperationTranslate: {
        double fromX = isIdentity(from) ? 0 : from->translate.x;
        double fromY = isIdentity(from) ? 0 : from->translate.y;
        double fromZ = isIdentity(from) ? 0 : from->translate.z;
         double toX = isIdentity(to) ? 0 : to->translate.x;
         double toY = isIdentity(to) ? 0 : to->translate.y;
         double toZ = isIdentity(to) ? 0 : to->translate.z;
        toReturn.translate3d(blendDoubles(fromX, toX, progress),
                             blendDoubles(fromY, toY, progress),
                             blendDoubles(fromZ, toZ, progress));
         break;
     }
     case WebTransformOperation::WebTransformOperationRotate: {
        double axisX = 0;
        double axisY = 0;
        double axisZ = 1;
         double fromAngle = 0;
         double toAngle = isIdentity(to) ? 0 : to->rotate.angle;
         if (shareSameAxis(from, to, axisX, axisY, axisZ, fromAngle))
            toReturn.rotate3d(axisX, axisY, axisZ, blendDoubles(fromAngle, toAngle, progress));
         else {
             WebTransformationMatrix toMatrix;
             if (!isIdentity(to))
                 toMatrix = to->matrix;
             WebTransformationMatrix fromMatrix;
             if (!isIdentity(from))
                 fromMatrix = from->matrix;
            toReturn = toMatrix;
            toReturn.blend(fromMatrix, progress);
         }
         break;
     }
    case WebTransformOperation::WebTransformOperationScale: {
        double fromX = isIdentity(from) ? 1 : from->scale.x;
        double fromY = isIdentity(from) ? 1 : from->scale.y;
        double fromZ = isIdentity(from) ? 1 : from->scale.z;
         double toX = isIdentity(to) ? 1 : to->scale.x;
         double toY = isIdentity(to) ? 1 : to->scale.y;
         double toZ = isIdentity(to) ? 1 : to->scale.z;
        toReturn.scale3d(blendDoubles(fromX, toX, progress),
                         blendDoubles(fromY, toY, progress),
                         blendDoubles(fromZ, toZ, progress));
         break;
     }
     case WebTransformOperation::WebTransformOperationSkew: {
         double fromX = isIdentity(from) ? 0 : from->skew.x;
         double fromY = isIdentity(from) ? 0 : from->skew.y;
         double toX = isIdentity(to) ? 0 : to->skew.x;
         double toY = isIdentity(to) ? 0 : to->skew.y;
        toReturn.skewX(blendDoubles(fromX, toX, progress));
        toReturn.skewY(blendDoubles(fromY, toY, progress));
         break;
     }
     case WebTransformOperation::WebTransformOperationPerspective: {
         double fromPerspectiveDepth = isIdentity(from) ? numeric_limits<double>::max() : from->perspectiveDepth;
         double toPerspectiveDepth = isIdentity(to) ? numeric_limits<double>::max() : to->perspectiveDepth;
        toReturn.applyPerspective(blendDoubles(fromPerspectiveDepth, toPerspectiveDepth, progress));
         break;
     }
     case WebTransformOperation::WebTransformOperationMatrix: {
        WebTransformationMatrix toMatrix;
        if (!isIdentity(to))
            toMatrix = to->matrix;
         WebTransformationMatrix fromMatrix;
         if (!isIdentity(from))
             fromMatrix = from->matrix;
        toReturn = toMatrix;
        toReturn.blend(fromMatrix, progress);
         break;
     }
     case WebTransformOperation::WebTransformOperationIdentity:
         break;
     }
 
    return toReturn;
 }
