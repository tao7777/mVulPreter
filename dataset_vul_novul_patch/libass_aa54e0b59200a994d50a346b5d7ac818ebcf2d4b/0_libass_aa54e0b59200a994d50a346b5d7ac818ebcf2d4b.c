static bool check_allocations(ASS_Shaper *shaper, size_t new_size)
{
    if (new_size > shaper->n_glyphs) {
        if (!ASS_REALLOC_ARRAY(shaper->event_text, new_size) ||
            !ASS_REALLOC_ARRAY(shaper->ctypes, new_size) ||
             !ASS_REALLOC_ARRAY(shaper->emblevels, new_size) ||
             !ASS_REALLOC_ARRAY(shaper->cmap, new_size))
             return false;
        shaper->n_glyphs = new_size;
     }
     return true;
 }
