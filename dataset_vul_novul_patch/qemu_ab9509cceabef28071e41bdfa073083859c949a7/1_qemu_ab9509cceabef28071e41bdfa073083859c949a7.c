static int interface_get_command(QXLInstance *sin, struct QXLCommandExt *ext)
{
    SimpleSpiceDisplay *ssd = container_of(sin, SimpleSpiceDisplay, qxl);
     info->num_memslots = NUM_MEMSLOTS;
     info->num_memslots_groups = NUM_MEMSLOTS_GROUPS;
     info->internal_groupslot_id = 0;
    info->qxl_ram_size = ssd->bufsize;
     info->n_surfaces = ssd->num_surfaces;
 }
