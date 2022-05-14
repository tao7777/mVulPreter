do_decrypt (const RIJNDAEL_context *ctx, unsigned char *bx,
            const unsigned char *ax)
 {
 #ifdef USE_AMD64_ASM
   return _gcry_aes_amd64_decrypt_block(ctx->keyschdec, bx, ax, ctx->rounds,
				       dec_tables.T);
 #elif defined(USE_ARM_ASM)
   return _gcry_aes_arm_decrypt_block(ctx->keyschdec, bx, ax, ctx->rounds,
				     dec_tables.T);
 #else
   return do_decrypt_fn (ctx, bx, ax);
 #endif /*!USE_ARM_ASM && !USE_AMD64_ASM*/
}
