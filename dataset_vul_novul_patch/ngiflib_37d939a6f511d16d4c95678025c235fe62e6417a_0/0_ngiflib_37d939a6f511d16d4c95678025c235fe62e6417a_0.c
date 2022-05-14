static void WritePixels(struct ngiflib_img * i, struct ngiflib_decode_context * context, const u8 * pixels, u16 n) {
	u16 tocopy;	
	struct ngiflib_gif * p = i->parent;

	while(n > 0) {
		tocopy = (context->Xtogo < n) ? context->Xtogo : n;
		if(!i->gce.transparent_flag) {
#ifndef NGIFLIB_INDEXED_ONLY
			if(p->mode & NGIFLIB_MODE_INDEXED) {
#endif /* NGIFLIB_INDEXED_ONLY */
				ngiflib_memcpy(context->frbuff_p.p8, pixels, tocopy);
				pixels += tocopy;
				context->frbuff_p.p8 += tocopy;
#ifndef NGIFLIB_INDEXED_ONLY
			} else {
				int j;
				for(j = (int)tocopy; j > 0; j--) {
					*(context->frbuff_p.p32++) =
					   GifIndexToTrueColor(i->palette, *pixels++);
				}
			}
#endif /* NGIFLIB_INDEXED_ONLY */
		} else {
			int j;
#ifndef NGIFLIB_INDEXED_ONLY
			if(p->mode & NGIFLIB_MODE_INDEXED) {
#endif /* NGIFLIB_INDEXED_ONLY */
				for(j = (int)tocopy; j > 0; j--) {
					if(*pixels != i->gce.transparent_color) *context->frbuff_p.p8 = *pixels;
					pixels++;
					context->frbuff_p.p8++;
				}
#ifndef NGIFLIB_INDEXED_ONLY
			} else {
				for(j = (int)tocopy; j > 0; j--) {
					if(*pixels != i->gce.transparent_color) {
						*context->frbuff_p.p32 = GifIndexToTrueColor(i->palette, *pixels);
					}
					pixels++;
					context->frbuff_p.p32++;
				}
			}
#endif /* NGIFLIB_INDEXED_ONLY */
		}
		context->Xtogo -= tocopy;
		if(context->Xtogo == 0) {
			#ifdef NGIFLIB_ENABLE_CALLBACKS
			if(p->line_cb) p->line_cb(p, context->line_p, context->curY);
			#endif /* NGIFLIB_ENABLE_CALLBACKS */
			context->Xtogo = i->width;
			switch(context->pass) {
			case 0:
				context->curY++;
 				break;
 			case 1:	/* 1st pass : every eighth row starting from 0 */
 				context->curY += 8;
 				break;
 			case 2:	/* 2nd pass : every eighth row starting from 4 */
 				context->curY += 8;
 				break;
 			case 3:	/* 3rd pass : every fourth row starting from 2 */
 				context->curY += 4;
 				break;
 			case 4:	/* 4th pass : every odd row */
 				context->curY += 2;
 				break;
 			}
			while(context->pass > 0 && context->pass < 4 &&
			      context->curY >= p->height) {
				switch(++context->pass) {
				case 2:	/* 2nd pass : every eighth row starting from 4 */
					context->curY = i->posY + 4;
					break;
				case 3:	/* 3rd pass : every fourth row starting from 2 */
					context->curY = i->posY + 2;
					break;
				case 4:	/* 4th pass : every odd row */
					context->curY = i->posY + 1;
					break;
				}
			}
 #ifndef NGIFLIB_INDEXED_ONLY
 			if(p->mode & NGIFLIB_MODE_INDEXED) {
 #endif /* NGIFLIB_INDEXED_ONLY */
				#ifdef NGIFLIB_ENABLE_CALLBACKS
				context->line_p.p8 = p->frbuff.p8 + (u32)context->curY*p->width;
				context->frbuff_p.p8 = context->line_p.p8 + i->posX;
				#else
				context->frbuff_p.p8 = p->frbuff.p8 + (u32)context->curY*p->width + i->posX;
				#endif /* NGIFLIB_ENABLE_CALLBACKS */
#ifndef NGIFLIB_INDEXED_ONLY
			} else {
				#ifdef NGIFLIB_ENABLE_CALLBACKS
				context->line_p.p32 = p->frbuff.p32 + (u32)context->curY*p->width;
				context->frbuff_p.p32 = context->line_p.p32 + i->posX;
				#else
				context->frbuff_p.p32 = p->frbuff.p32 + (u32)context->curY*p->width + i->posX;
				#endif /* NGIFLIB_ENABLE_CALLBACKS */
			}
#endif /* NGIFLIB_INDEXED_ONLY */
		}
		n -= tocopy;
	}
}
