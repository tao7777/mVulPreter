do_encrypt (const RIJNDAEL_context *ctx,
            unsigned char *bx, const unsigned char *ax)
 {
 #ifdef USE_AMD64_ASM
   return _gcry_aes_amd64_encrypt_block(ctx->keyschenc, bx, ax, ctx->rounds,
				       enc_tables.T);
 #elif defined(USE_ARM_ASM)
  return _gcry_aes_arm_encrypt_block(ctx->keyschenc, bx, ax, ctx->rounds,
				     enc_tables.T);
 #else
   return do_encrypt_fn (ctx, bx, ax);
 #endif /* !USE_ARM_ASM && !USE_AMD64_ASM*/
}
