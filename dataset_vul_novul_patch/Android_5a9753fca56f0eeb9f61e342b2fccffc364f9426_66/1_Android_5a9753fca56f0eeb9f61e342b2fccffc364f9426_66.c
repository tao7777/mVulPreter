 static bool compare_img(const vpx_image_t *img1,
                         const vpx_image_t *img2) {
   bool match = (img1->fmt == img2->fmt) &&
                (img1->d_w == img2->d_w) &&
                (img1->d_h == img2->d_h);
 
 const unsigned int width_y  = img1->d_w;
 const unsigned int height_y = img1->d_h;
 unsigned int i;
 for (i = 0; i < height_y; ++i)
    match = (memcmp(img1->planes[VPX_PLANE_Y] + i * img1->stride[VPX_PLANE_Y],
                    img2->planes[VPX_PLANE_Y] + i * img2->stride[VPX_PLANE_Y],
                    width_y) == 0) && match;
 const unsigned int width_uv  = (img1->d_w + 1) >> 1;
 const unsigned int height_uv = (img1->d_h + 1) >> 1;
 for (i = 0; i <  height_uv; ++i)
    match = (memcmp(img1->planes[VPX_PLANE_U] + i * img1->stride[VPX_PLANE_U],
                    img2->planes[VPX_PLANE_U] + i * img2->stride[VPX_PLANE_U],
                    width_uv) == 0) && match;
 for (i = 0; i < height_uv; ++i)
    match = (memcmp(img1->planes[VPX_PLANE_V] + i * img1->stride[VPX_PLANE_V],
                    img2->planes[VPX_PLANE_V] + i * img2->stride[VPX_PLANE_V],
                    width_uv) == 0) && match;

   return match;
 }
