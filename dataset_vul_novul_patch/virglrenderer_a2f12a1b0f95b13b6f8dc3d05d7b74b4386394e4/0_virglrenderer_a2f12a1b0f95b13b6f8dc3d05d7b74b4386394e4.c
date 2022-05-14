static struct vrend_linked_shader_program *add_shader_program(struct vrend_context *ctx,
                                                              struct vrend_shader *vs,
                                                              struct vrend_shader *fs,
                                                              struct vrend_shader *gs)
{
   struct vrend_linked_shader_program *sprog = CALLOC_STRUCT(vrend_linked_shader_program);
   char name[16];
   int i;
   GLuint prog_id;
   GLint lret;
   int id;
   int last_shader;
   if (!sprog)
      return NULL;

   /* need to rewrite VS code to add interpolation params */
   if ((gs && gs->compiled_fs_id != fs->id) ||
       (!gs && vs->compiled_fs_id != fs->id)) {
      bool ret;

      if (gs)
         vrend_patch_vertex_shader_interpolants(gs->glsl_prog,
                                                &gs->sel->sinfo,
                                                &fs->sel->sinfo, true, fs->key.flatshade);
      else
         vrend_patch_vertex_shader_interpolants(vs->glsl_prog,
                                                &vs->sel->sinfo,
                                                &fs->sel->sinfo, false, fs->key.flatshade);
      ret = vrend_compile_shader(ctx, gs ? gs : vs);
      if (ret == false) {
         glDeleteShader(gs ? gs->id : vs->id);
         free(sprog);
         return NULL;
      }
      if (gs)
         gs->compiled_fs_id = fs->id;
      else
         vs->compiled_fs_id = fs->id;
   }

   prog_id = glCreateProgram();
   glAttachShader(prog_id, vs->id);
   if (gs) {
      if (gs->id > 0)
         glAttachShader(prog_id, gs->id);
      set_stream_out_varyings(prog_id, &gs->sel->sinfo);
   }
   else
      set_stream_out_varyings(prog_id, &vs->sel->sinfo);
   glAttachShader(prog_id, fs->id);

   if (fs->sel->sinfo.num_outputs > 1) {
      if (util_blend_state_is_dual(&ctx->sub->blend_state, 0)) {
         glBindFragDataLocationIndexed(prog_id, 0, 0, "fsout_c0");
         glBindFragDataLocationIndexed(prog_id, 0, 1, "fsout_c1");
         sprog->dual_src_linked = true;
      } else {
         glBindFragDataLocationIndexed(prog_id, 0, 0, "fsout_c0");
         glBindFragDataLocationIndexed(prog_id, 1, 0, "fsout_c1");
         sprog->dual_src_linked = false;
      }
   } else
      sprog->dual_src_linked = false;

   if (vrend_state.have_vertex_attrib_binding) {
      uint32_t mask = vs->sel->sinfo.attrib_input_mask;
      while (mask) {
         i = u_bit_scan(&mask);
         snprintf(name, 10, "in_%d", i);
         glBindAttribLocation(prog_id, i, name);
      }
   }

   glLinkProgram(prog_id);

   glGetProgramiv(prog_id, GL_LINK_STATUS, &lret);
   if (lret == GL_FALSE) {
      char infolog[65536];
      int len;
      glGetProgramInfoLog(prog_id, 65536, &len, infolog);
      fprintf(stderr,"got error linking\n%s\n", infolog);
      /* dump shaders */
      report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_SHADER, 0);
      fprintf(stderr,"vert shader: %d GLSL\n%s\n", vs->id, vs->glsl_prog);
      if (gs)
          fprintf(stderr,"geom shader: %d GLSL\n%s\n", gs->id, gs->glsl_prog);
       fprintf(stderr,"frag shader: %d GLSL\n%s\n", fs->id, fs->glsl_prog);
       glDeleteProgram(prog_id);
      free(sprog);
       return NULL;
    }
   sprog->ss[PIPE_SHADER_FRAGMENT] = fs;
   sprog->ss[PIPE_SHADER_GEOMETRY] = gs;

   list_add(&sprog->sl[PIPE_SHADER_VERTEX], &vs->programs);
   list_add(&sprog->sl[PIPE_SHADER_FRAGMENT], &fs->programs);
   if (gs)
      list_add(&sprog->sl[PIPE_SHADER_GEOMETRY], &gs->programs);

   last_shader = gs ? PIPE_SHADER_GEOMETRY : PIPE_SHADER_FRAGMENT;
   sprog->id = prog_id;

   list_addtail(&sprog->head, &ctx->sub->programs);

   if (fs->key.pstipple_tex)
      sprog->fs_stipple_loc = glGetUniformLocation(prog_id, "pstipple_sampler");
   else
      sprog->fs_stipple_loc = -1;
   sprog->vs_ws_adjust_loc = glGetUniformLocation(prog_id, "winsys_adjust");
   for (id = PIPE_SHADER_VERTEX; id <= last_shader; id++) {
      if (sprog->ss[id]->sel->sinfo.samplers_used_mask) {
         uint32_t mask = sprog->ss[id]->sel->sinfo.samplers_used_mask;
         int nsamp = util_bitcount(sprog->ss[id]->sel->sinfo.samplers_used_mask);
         int index;
         sprog->shadow_samp_mask[id] = sprog->ss[id]->sel->sinfo.shadow_samp_mask;
         if (sprog->ss[id]->sel->sinfo.shadow_samp_mask) {
            sprog->shadow_samp_mask_locs[id] = calloc(nsamp, sizeof(uint32_t));
            sprog->shadow_samp_add_locs[id] = calloc(nsamp, sizeof(uint32_t));
         } else {
            sprog->shadow_samp_mask_locs[id] = sprog->shadow_samp_add_locs[id] = NULL;
         }
         sprog->samp_locs[id] = calloc(nsamp, sizeof(uint32_t));
         if (sprog->samp_locs[id]) {
            const char *prefix = pipe_shader_to_prefix(id);
            index = 0;
            while(mask) {
               i = u_bit_scan(&mask);
               snprintf(name, 10, "%ssamp%d", prefix, i);
               sprog->samp_locs[id][index] = glGetUniformLocation(prog_id, name);
               if (sprog->ss[id]->sel->sinfo.shadow_samp_mask & (1 << i)) {
                  snprintf(name, 14, "%sshadmask%d", prefix, i);
                  sprog->shadow_samp_mask_locs[id][index] = glGetUniformLocation(prog_id, name);
                  snprintf(name, 14, "%sshadadd%d", prefix, i);
                  sprog->shadow_samp_add_locs[id][index] = glGetUniformLocation(prog_id, name);
               }
               index++;
            }
         }
      } else {
         sprog->samp_locs[id] = NULL;
         sprog->shadow_samp_mask_locs[id] = NULL;
         sprog->shadow_samp_add_locs[id] = NULL;
         sprog->shadow_samp_mask[id] = 0;
      }
      sprog->samplers_used_mask[id] = sprog->ss[id]->sel->sinfo.samplers_used_mask;
   }

   for (id = PIPE_SHADER_VERTEX; id <= last_shader; id++) {
      if (sprog->ss[id]->sel->sinfo.num_consts) {
         sprog->const_locs[id] = calloc(sprog->ss[id]->sel->sinfo.num_consts, sizeof(uint32_t));
         if (sprog->const_locs[id]) {
            const char *prefix = pipe_shader_to_prefix(id);
            for (i = 0; i < sprog->ss[id]->sel->sinfo.num_consts; i++) {
               snprintf(name, 16, "%sconst0[%d]", prefix, i);
               sprog->const_locs[id][i] = glGetUniformLocation(prog_id, name);
            }
         }
      } else
         sprog->const_locs[id] = NULL;
   }

   if (!vrend_state.have_vertex_attrib_binding) {
      if (vs->sel->sinfo.num_inputs) {
         sprog->attrib_locs = calloc(vs->sel->sinfo.num_inputs, sizeof(uint32_t));
         if (sprog->attrib_locs) {
            for (i = 0; i < vs->sel->sinfo.num_inputs; i++) {
               snprintf(name, 10, "in_%d", i);
               sprog->attrib_locs[i] = glGetAttribLocation(prog_id, name);
            }
         }
      } else
         sprog->attrib_locs = NULL;
   }

   for (id = PIPE_SHADER_VERTEX; id <= last_shader; id++) {
      if (sprog->ss[id]->sel->sinfo.num_ubos) {
         const char *prefix = pipe_shader_to_prefix(id);

         sprog->ubo_locs[id] = calloc(sprog->ss[id]->sel->sinfo.num_ubos, sizeof(uint32_t));
         for (i = 0; i < sprog->ss[id]->sel->sinfo.num_ubos; i++) {
            snprintf(name, 16, "%subo%d", prefix, i + 1);
            sprog->ubo_locs[id][i] = glGetUniformBlockIndex(prog_id, name);
         }
      } else
         sprog->ubo_locs[id] = NULL;
   }

   if (vs->sel->sinfo.num_ucp) {
      for (i = 0; i < vs->sel->sinfo.num_ucp; i++) {
         snprintf(name, 10, "clipp[%d]", i);
         sprog->clip_locs[i] = glGetUniformLocation(prog_id, name);
      }
   }
   return sprog;
}
