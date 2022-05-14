static void setPathFromConvexPoints(SkPath* path, size_t numPoints, const FloatPoint* points)
{
    path->incReserve(numPoints);
    path->moveTo(WebCoreFloatToSkScalar(points[0].x()),
                 WebCoreFloatToSkScalar(points[0].y()));
    for (size_t i = 1; i < numPoints; ++i) {
         path->lineTo(WebCoreFloatToSkScalar(points[i].x()),
                      WebCoreFloatToSkScalar(points[i].y()));
     }

    /*  The code used to just blindly call this
            path->setIsConvex(true);
        But webkit can sometimes send us non-convex 4-point values, so we mark the path's
        convexity as unknown, so it will get computed by skia at draw time.
        See crbug.com 108605
    */
    SkPath::Convexity convexity = SkPath::kConvex_Convexity;
    if (numPoints == 4)
        convexity = SkPath::kUnknown_Convexity;
    path->setConvexity(convexity);
 }
