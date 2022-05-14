gs_pattern2_set_color(const gs_client_color * pcc, gs_gstate * pgs)
{
    gs_pattern2_instance_t * pinst = (gs_pattern2_instance_t *)pcc->pattern;
    gs_color_space * pcs = pinst->templat.Shading->params.ColorSpace;
    int code;
    uchar k, num_comps;
 
     pinst->saved->overprint_mode = pgs->overprint_mode;
     pinst->saved->overprint = pgs->overprint;

     num_comps = pgs->device->color_info.num_components;
     for (k = 0; k < num_comps; k++) {
        pgs->color_component_map.color_map[k] =
             pinst->saved->color_component_map.color_map[k];
     }
     code = pcs->type->set_overprint(pcs, pgs);
    return code;
}
