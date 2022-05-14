static int DecodeGifImg(struct ngiflib_img * i) {
	struct ngiflib_decode_context context;
	long npix;
	u8 * stackp;
	u8 * stack_top;
	u16 clr;
	u16 eof;
	u16 free;
	u16 act_code = 0;
	u16 old_code = 0;
	u16 read_byt;
	u16 ab_prfx[4096];
	u8 ab_suffx[4096];
	u8 ab_stack[4096];
	u8 flags;
	u8 casspecial = 0;

	if(!i) return -1;

	i->posX = GetWord(i->parent);	/* offsetX */
	i->posY = GetWord(i->parent);	/* offsetY */
	i->width = GetWord(i->parent);	/* SizeX   */
	i->height = GetWord(i->parent);	/* SizeY   */

	if((i->width > i->parent->width) || (i->height > i->parent->height)) {
#if !defined(NGIFLIB_NO_FILE)
		if(i->parent->log) fprintf(i->parent->log, "*** ERROR *** Image bigger than global GIF canvas !\n");
#endif
		return -1;
	}
	if((i->posX + i->width) > i->parent->width) {
#if !defined(NGIFLIB_NO_FILE)
		if(i->parent->log) fprintf(i->parent->log, "*** WARNING *** Adjusting X position\n");
#endif
		i->posX = i->parent->width - i->width;
	}
	if((i->posY + i->height) > i->parent->height) {
#if !defined(NGIFLIB_NO_FILE)
		if(i->parent->log) fprintf(i->parent->log, "*** WARNING *** Adjusting Y position\n");
#endif
		i->posY = i->parent->height - i->height;
	}
	context.Xtogo = i->width;
	context.curY = i->posY;
#ifdef NGIFLIB_INDEXED_ONLY
	#ifdef NGIFLIB_ENABLE_CALLBACKS
	context.line_p.p8 = i->parent->frbuff.p8 + (u32)i->posY*i->parent->width;
	context.frbuff_p.p8 = context.line_p.p8 + i->posX;
	#else
	context.frbuff_p.p8 = i->parent->frbuff.p8 + (u32)i->posY*i->parent->width + i->posX;
	#endif /* NGIFLIB_ENABLE_CALLBACKS */
#else
	if(i->parent->mode & NGIFLIB_MODE_INDEXED) {
		#ifdef NGIFLIB_ENABLE_CALLBACKS
		context.line_p.p8 = i->parent->frbuff.p8 + (u32)i->posY*i->parent->width;
		context.frbuff_p.p8 = context.line_p.p8 + i->posX;
		#else
		context.frbuff_p.p8 = i->parent->frbuff.p8 + (u32)i->posY*i->parent->width + i->posX;
		#endif /* NGIFLIB_ENABLE_CALLBACKS */
	} else {
		#ifdef NGIFLIB_ENABLE_CALLBACKS
		context.line_p.p32 = i->parent->frbuff.p32 + (u32)i->posY*i->parent->width;
		context.frbuff_p.p32 = context.line_p.p32 + i->posX;
		#else
		context.frbuff_p.p32 = i->parent->frbuff.p32 + (u32)i->posY*i->parent->width + i->posX;
		#endif /* NGIFLIB_ENABLE_CALLBACKS */
	}
#endif /* NGIFLIB_INDEXED_ONLY */

	npix = (long)i->width * i->height;
	flags = GetByte(i->parent);
	i->interlaced = (flags & 64) >> 6;
	context.pass = i->interlaced ? 1 : 0;
	i->sort_flag = (flags & 32) >> 5;	/* is local palette sorted by color frequency ? */
	i->localpalbits = (flags & 7) + 1;
	if(flags&128) { /* palette locale */
		int k;
		int localpalsize = 1 << i->localpalbits;
#if !defined(NGIFLIB_NO_FILE)
		if(i->parent && i->parent->log) fprintf(i->parent->log, "Local palette\n");
#endif /* !defined(NGIFLIB_NO_FILE) */
		i->palette = (struct ngiflib_rgb *)ngiflib_malloc(sizeof(struct ngiflib_rgb)*localpalsize);
		for(k=0; k<localpalsize; k++) {
			i->palette[k].r = GetByte(i->parent);
			i->palette[k].g = GetByte(i->parent);
			i->palette[k].b = GetByte(i->parent);
		}
#ifdef NGIFLIB_ENABLE_CALLBACKS
		if(i->parent->palette_cb) i->parent->palette_cb(i->parent, i->palette, localpalsize);
#endif /* NGIFLIB_ENABLE_CALLBACKS */
	} else {
		i->palette = i->parent->palette;
		i->localpalbits = i->parent->imgbits;
	}
	i->ncolors = 1 << i->localpalbits;
	
	i->imgbits = GetByte(i->parent);	/* LZW Minimum Code Size */

#if !defined(NGIFLIB_NO_FILE)
	if(i->parent && i->parent->log) {
		if(i->interlaced) fprintf(i->parent->log, "interlaced ");
		fprintf(i->parent->log, "img pos(%hu,%hu) size %hux%hu palbits=%hhu imgbits=%hhu ncolors=%hu\n",
	       i->posX, i->posY, i->width, i->height, i->localpalbits, i->imgbits, i->ncolors);
	}
#endif /* !defined(NGIFLIB_NO_FILE) */

	if(i->imgbits==1) {	/* fix for 1bit images ? */
		i->imgbits = 2;
	}
	clr = 1 << i->imgbits;
	eof = clr + 1;
	free = clr + 2;
	context.nbbit = i->imgbits + 1;
	context.max = clr + clr - 1; /* (1 << context.nbbit) - 1 */
	stackp = stack_top = ab_stack + 4096;
	
	context.restbits = 0;	/* initialise le "buffer" de lecture */
	context.restbyte = 0;	/* des codes LZW */
	context.lbyte = 0;
	for(;;) {
		act_code = GetGifWord(i, &context);
		if(act_code==eof) {
#if !defined(NGIFLIB_NO_FILE)
			if(i->parent && i->parent->log) fprintf(i->parent->log, "End of image code\n");
#endif /* !defined(NGIFLIB_NO_FILE) */
			return 0;
		}
		if(npix==0) {
#if !defined(NGIFLIB_NO_FILE)
			if(i->parent && i->parent->log) fprintf(i->parent->log, "assez de pixels, On se casse !\n");
#endif /* !defined(NGIFLIB_NO_FILE) */
			return 1;
		}	
		if(act_code==clr) {
#if !defined(NGIFLIB_NO_FILE)
			if(i->parent && i->parent->log) fprintf(i->parent->log, "Code clear (free=%hu) npix=%ld\n", free, npix);
#endif /* !defined(NGIFLIB_NO_FILE) */
			/* clear */
			free = clr + 2;
			context.nbbit = i->imgbits + 1;
			context.max = clr + clr - 1; /* (1 << context.nbbit) - 1 */
 			act_code = GetGifWord(i, &context);
 			casspecial = (u8)act_code;
 			old_code = act_code;
			if(npix > 0) WritePixel(i, &context, casspecial);
			npix--;
 		} else {
 			read_byt = act_code;
 			if(act_code >= free) {	/* code pas encore dans alphabet */
/*				printf("Code pas dans alphabet : %d>=%d push %d\n", act_code, free, casspecial); */
				*(--stackp) = casspecial; /* dernier debut de chaine ! */
				act_code = old_code;
			}
/*			printf("actcode=%d\n", act_code); */
			while(act_code > clr) { /* code non concret */
				/* fillstackloop empile les suffixes ! */
				*(--stackp) = ab_suffx[act_code];
				act_code = ab_prfx[act_code];	/* prefixe */
			}
 			/* act_code est concret */
 			casspecial = (u8)act_code;	/* dernier debut de chaine ! */
 			*(--stackp) = casspecial;	/* push on stack */
			if(npix >= (stack_top - stackp)) {
				WritePixels(i, &context, stackp, stack_top - stackp);	/* unstack all pixels at once */
			} else if(npix > 0) {	/* "pixel overflow" */
				WritePixels(i, &context, stackp, npix);
			}
 			npix -= (stack_top - stackp);
 			stackp = stack_top;
 /*			putchar('\n'); */
			if(free < 4096) { /* la taille du dico est 4096 max ! */
				ab_prfx[free] = old_code;
				ab_suffx[free] = (u8)act_code;
				free++;
				if((free > context.max) && (context.nbbit < 12)) {
					context.nbbit++;	/* 1 bit de plus pour les codes LZW */
					context.max += context.max + 1;
				}
			}
			old_code = read_byt;
		}
			
	}
	return 0;
}
