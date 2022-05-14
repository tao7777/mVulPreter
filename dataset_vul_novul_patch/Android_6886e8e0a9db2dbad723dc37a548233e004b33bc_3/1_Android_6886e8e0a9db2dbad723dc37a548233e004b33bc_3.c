void vp8mt_de_alloc_temp_buffers(VP8D_COMP *pbi, int mb_rows)
{
    int i;
 
    if (pbi->b_multithreaded_rd)
    {
            vpx_free(pbi->mt_current_mb_col);
            pbi->mt_current_mb_col = NULL ;
 
        /* Free above_row buffers. */
        if (pbi->mt_yabove_row)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_yabove_row[i]);
                    pbi->mt_yabove_row[i] = NULL ;
            }
            vpx_free(pbi->mt_yabove_row);
            pbi->mt_yabove_row = NULL ;
        }
        if (pbi->mt_uabove_row)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_uabove_row[i]);
                    pbi->mt_uabove_row[i] = NULL ;
            }
            vpx_free(pbi->mt_uabove_row);
            pbi->mt_uabove_row = NULL ;
        }
        if (pbi->mt_vabove_row)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_vabove_row[i]);
                    pbi->mt_vabove_row[i] = NULL ;
            }
            vpx_free(pbi->mt_vabove_row);
            pbi->mt_vabove_row = NULL ;
        }
        /* Free left_col buffers. */
        if (pbi->mt_yleft_col)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_yleft_col[i]);
                    pbi->mt_yleft_col[i] = NULL ;
            }
            vpx_free(pbi->mt_yleft_col);
            pbi->mt_yleft_col = NULL ;
        }
        if (pbi->mt_uleft_col)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_uleft_col[i]);
                    pbi->mt_uleft_col[i] = NULL ;
            }
            vpx_free(pbi->mt_uleft_col);
            pbi->mt_uleft_col = NULL ;
        }
        if (pbi->mt_vleft_col)
        {
            for (i=0; i< mb_rows; i++)
            {
                    vpx_free(pbi->mt_vleft_col[i]);
                    pbi->mt_vleft_col[i] = NULL ;
            }
            vpx_free(pbi->mt_vleft_col);
            pbi->mt_vleft_col = NULL ;
        }
     }
 }
