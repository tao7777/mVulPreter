parse_instruction(
   struct translate_ctx *ctx,
   boolean has_label )
{
   uint i;
   uint saturate = 0;
   const struct tgsi_opcode_info *info;
   struct tgsi_full_instruction inst;
   const char *cur;
   uint advance;

   inst = tgsi_default_full_instruction();

   /* Parse predicate.
    */
   eat_opt_white( &ctx->cur );
   if (*ctx->cur == '(') {
      uint file;
      int index;
      uint swizzle[4];
      boolean parsed_swizzle;

      inst.Instruction.Predicate = 1;

      ctx->cur++;
      if (*ctx->cur == '!') {
         ctx->cur++;
         inst.Predicate.Negate = 1;
      }

      if (!parse_register_1d( ctx, &file, &index ))
         return FALSE;

      if (parse_optional_swizzle( ctx, swizzle, &parsed_swizzle, 4 )) {
         if (parsed_swizzle) {
            inst.Predicate.SwizzleX = swizzle[0];
            inst.Predicate.SwizzleY = swizzle[1];
            inst.Predicate.SwizzleZ = swizzle[2];
            inst.Predicate.SwizzleW = swizzle[3];
         }
      }

      if (*ctx->cur != ')') {
         report_error( ctx, "Expected `)'" );
         return FALSE;
      }

      ctx->cur++;
   }

   /* Parse instruction name.
    */
   eat_opt_white( &ctx->cur );
   for (i = 0; i < TGSI_OPCODE_LAST; i++) {
      cur = ctx->cur;

      info = tgsi_get_opcode_info( i );
      if (match_inst(&cur, &saturate, info)) {
         if (info->num_dst + info->num_src + info->is_tex == 0) {
            ctx->cur = cur;
            break;
         }
         else if (*cur == '\0' || eat_white( &cur )) {
            ctx->cur = cur;
            break;
         }
      }
   }
   if (i == TGSI_OPCODE_LAST) {
      if (has_label)
         report_error( ctx, "Unknown opcode" );
      else
         report_error( ctx, "Expected `DCL', `IMM' or a label" );
      return FALSE;
   }

   inst.Instruction.Opcode = i;
   inst.Instruction.Saturate = saturate;
   inst.Instruction.NumDstRegs = info->num_dst;
   inst.Instruction.NumSrcRegs = info->num_src;

   if (i >= TGSI_OPCODE_SAMPLE && i <= TGSI_OPCODE_GATHER4) {
      /*
       * These are not considered tex opcodes here (no additional
       * target argument) however we're required to set the Texture
       * bit so we can set the number of tex offsets.
       */
      inst.Instruction.Texture = 1;
      inst.Texture.Texture = TGSI_TEXTURE_UNKNOWN;
   }

   /* Parse instruction operands.
    */
   for (i = 0; i < info->num_dst + info->num_src + info->is_tex; i++) {
      if (i > 0) {
         eat_opt_white( &ctx->cur );
         if (*ctx->cur != ',') {
            report_error( ctx, "Expected `,'" );
            return FALSE;
         }
         ctx->cur++;
         eat_opt_white( &ctx->cur );
      }

      if (i < info->num_dst) {
         if (!parse_dst_operand( ctx, &inst.Dst[i] ))
            return FALSE;
      }
      else if (i < info->num_dst + info->num_src) {
         if (!parse_src_operand( ctx, &inst.Src[i - info->num_dst] ))
            return FALSE;
      }
      else {
         uint j;

         for (j = 0; j < TGSI_TEXTURE_COUNT; j++) {
            if (str_match_nocase_whole( &ctx->cur, tgsi_texture_names[j] )) {
               inst.Instruction.Texture = 1;
               inst.Texture.Texture = j;
               break;
            }
         }
         if (j == TGSI_TEXTURE_COUNT) {
            report_error( ctx, "Expected texture target" );
            return FALSE;
         }
      }
   }
 
    cur = ctx->cur;
    eat_opt_white( &cur );
   for (i = 0; inst.Instruction.Texture && *cur == ',' && i < TGSI_FULL_MAX_TEX_OFFSETS; i++) {
          cur++;
          eat_opt_white( &cur );
          ctx->cur = cur;
         if (!parse_texoffset_operand( ctx, &inst.TexOffsets[i] ))
            return FALSE;
         cur = ctx->cur;
         eat_opt_white( &cur );
   }
   inst.Texture.NumOffsets = i;

   cur = ctx->cur;
   eat_opt_white( &cur );
   if (info->is_branch && *cur == ':') {
      uint target;

      cur++;
      eat_opt_white( &cur );
      if (!parse_uint( &cur, &target )) {
         report_error( ctx, "Expected a label" );
         return FALSE;
      }
      inst.Instruction.Label = 1;
      inst.Label.Label = target;
      ctx->cur = cur;
   }

   advance = tgsi_build_full_instruction(
      &inst,
      ctx->tokens_cur,
      ctx->header,
      (uint) (ctx->tokens_end - ctx->tokens_cur) );
   if (advance == 0)
      return FALSE;
   ctx->tokens_cur += advance;

   return TRUE;
}
