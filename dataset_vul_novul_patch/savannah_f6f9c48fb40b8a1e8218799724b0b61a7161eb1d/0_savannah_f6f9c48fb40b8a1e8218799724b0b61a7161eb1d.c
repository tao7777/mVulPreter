get_cdtext_generic (void *p_user_data)
{
  generic_img_private_t *p_env = p_user_data;
  uint8_t *p_cdtext_data = NULL;
  size_t  len;

  if (!p_env) return NULL;

  if (p_env->b_cdtext_error) return NULL;

  if (NULL == p_env->cdtext) {
    p_cdtext_data = read_cdtext_generic (p_env);

    if (NULL != p_cdtext_data) {
      len = CDIO_MMC_GET_LEN16(p_cdtext_data)-2;
      p_env->cdtext = cdtext_init();
 
       if(len <= 0 || 0 != cdtext_data_init (p_env->cdtext, &p_cdtext_data[4], len)) {
         p_env->b_cdtext_error = true;
         free(p_env->cdtext);
         p_env->cdtext = NULL;
       }
      }

      free(p_cdtext_data);
    }
  }
