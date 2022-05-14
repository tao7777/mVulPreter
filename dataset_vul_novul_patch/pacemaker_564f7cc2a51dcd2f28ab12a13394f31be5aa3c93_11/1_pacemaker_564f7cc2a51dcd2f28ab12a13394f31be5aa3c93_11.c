mainloop_destroy_trigger(crm_trigger_t * source)
{
     source->trigger = FALSE;
     if (source->id > 0) {
         g_source_remove(source->id);
     }
     return TRUE;
 }
