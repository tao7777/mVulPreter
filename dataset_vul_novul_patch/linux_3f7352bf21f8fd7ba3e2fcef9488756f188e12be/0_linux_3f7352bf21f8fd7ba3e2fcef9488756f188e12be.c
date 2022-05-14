void bpf_int_jit_compile(struct bpf_prog *prog)
{
	struct bpf_binary_header *header = NULL;
	int proglen, oldproglen = 0;
	struct jit_context ctx = {};
	u8 *image = NULL;
	int *addrs;
	int pass;
	int i;

	if (!bpf_jit_enable)
		return;

	if (!prog || !prog->len)
		return;

	addrs = kmalloc(prog->len * sizeof(*addrs), GFP_KERNEL);
	if (!addrs)
		return;

	/* Before first pass, make a rough estimation of addrs[]
	 * each bpf instruction is translated to less than 64 bytes
	 */
	for (proglen = 0, i = 0; i < prog->len; i++) {
		proglen += 64;
		addrs[i] = proglen;
 	}
 	ctx.cleanup_addr = proglen;
 
	/* JITed image shrinks with every pass and the loop iterates
	 * until the image stops shrinking. Very large bpf programs
	 * may converge on the last pass. In such case do one more
	 * pass to emit the final image
	 */
	for (pass = 0; pass < 10 || image; pass++) {
 		proglen = do_jit(prog, addrs, image, oldproglen, &ctx);
 		if (proglen <= 0) {
 			image = NULL;
			if (header)
				bpf_jit_binary_free(header);
			goto out;
		}
		if (image) {
			if (proglen != oldproglen) {
				pr_err("bpf_jit: proglen=%d != oldproglen=%d\n",
				       proglen, oldproglen);
				goto out;
			}
			break;
		}
		if (proglen == oldproglen) {
			header = bpf_jit_binary_alloc(proglen, &image,
						      1, jit_fill_hole);
			if (!header)
				goto out;
		}
		oldproglen = proglen;
	}

	if (bpf_jit_enable > 1)
		bpf_jit_dump(prog->len, proglen, 0, image);

	if (image) {
		bpf_flush_icache(header, image + proglen);
		set_memory_ro((unsigned long)header, header->pages);
		prog->bpf_func = (void *)image;
		prog->jited = true;
	}
out:
	kfree(addrs);
}
