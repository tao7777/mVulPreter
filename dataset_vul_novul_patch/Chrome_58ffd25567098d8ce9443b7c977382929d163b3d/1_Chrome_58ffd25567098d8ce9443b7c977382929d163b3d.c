static void setPathFromConvexPoints(SkPath* path, size_t numPoints, const FloatPoint* points)
{
    path->incReserve(numPoints);
    path->moveTo(WebCoreFloatToSkScalar(points[0].x()),
                 WebCoreFloatToSkScalar(points[0].y()));
    for (size_t i = 1; i < numPoints; ++i) {
         path->lineTo(WebCoreFloatToSkScalar(points[i].x()),
                      WebCoreFloatToSkScalar(points[i].y()));
     }
    path->setIsConvex(true);
 }
