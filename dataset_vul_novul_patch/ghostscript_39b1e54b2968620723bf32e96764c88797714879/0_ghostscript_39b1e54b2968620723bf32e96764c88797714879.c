 set_text_distance(gs_point *pdist, double dx, double dy, const gs_matrix *pmat)
 {
    int code;
     double rounded;
 
    if (dx > 1e38 || dy > 1e38)
        code = gs_error_undefinedresult;
    else
        code = gs_distance_transform_inverse(dx, dy, pmat, pdist);

     if (code == gs_error_undefinedresult) {
         /* The CTM is degenerate.
            Can't know the distance in user space.
    } else if (code < 0)
        return code;
    /* If the distance is very close to integers, round it. */
    if (fabs(pdist->x - (rounded = floor(pdist->x + 0.5))) < 0.0005)
        pdist->x = rounded;
    if (fabs(pdist->y - (rounded = floor(pdist->y + 0.5))) < 0.0005)
        pdist->y = rounded;
    return 0;
}
