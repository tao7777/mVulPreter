XGetFeedbackControl(
    register Display	*dpy,
    XDevice		*dev,
    int			*num_feedbacks)
{
    XFeedbackState *Feedback = NULL;
     XFeedbackState *Sav = NULL;
     xFeedbackState *f = NULL;
     xFeedbackState *sav = NULL;
    char *end = NULL;
     xGetFeedbackControlReq *req;
     xGetFeedbackControlReply rep;
     XExtDisplayInfo *info = XInput_find_display(dpy);
    LockDisplay(dpy);
    if (_XiCheckExtInit(dpy, XInput_Initial_Release, info) == -1)
        return NULL;

    GetReq(GetFeedbackControl, req);
    req->reqType = info->codes->major_opcode;
    req->ReqType = X_GetFeedbackControl;
    req->deviceid = dev->device_id;

    if (!_XReply(dpy, (xReply *) & rep, 0, xFalse))
	goto out;

    if (rep.length > 0) {
	unsigned long nbytes;
	size_t size = 0;
	int i;

	*num_feedbacks = rep.num_feedbacks;

	if (rep.length < (INT_MAX >> 2)) {
	    nbytes = rep.length << 2;
	    f = Xmalloc(nbytes);
	}
	if (!f) {
	    _XEatDataWords(dpy, rep.length);
	    goto out;
 	    goto out;
 	}
 	sav = f;
	end = (char *)f + nbytes;
 	_XRead(dpy, (char *)f, nbytes);
 
 	for (i = 0; i < *num_feedbacks; i++) {
	    if ((char *)f + sizeof(*f) > end ||
	        f->length == 0 || f->length > nbytes)
 		goto out;
 	    nbytes -= f->length;
 
		break;
	    case PtrFeedbackClass:
		size += sizeof(XPtrFeedbackState);
		break;
	    case IntegerFeedbackClass:
		size += sizeof(XIntegerFeedbackState);
		break;
	    case StringFeedbackClass:
	    {
		xStringFeedbackState *strf = (xStringFeedbackState *) f;
 	    case StringFeedbackClass:
 	    {
 		xStringFeedbackState *strf = (xStringFeedbackState *) f;
		if ((char *)f + sizeof(*strf) > end)
		    goto out;
 		size += sizeof(XStringFeedbackState) +
 		    (strf->num_syms_supported * sizeof(KeySym));
 	    }
		size += sizeof(XBellFeedbackState);
		break;
	    default:
		size += f->length;
		break;
	    }
	    if (size > INT_MAX)
		goto out;
	    f = (xFeedbackState *) ((char *)f + f->length);
	}

	Feedback = Xmalloc(size);
	if (!Feedback)
	    goto out;

	Sav = Feedback;

	f = sav;
	for (i = 0; i < *num_feedbacks; i++) {
	    switch (f->class) {
	    case KbdFeedbackClass:
	    {
		xKbdFeedbackState *k;
		XKbdFeedbackState *K;

		k = (xKbdFeedbackState *) f;
		K = (XKbdFeedbackState *) Feedback;

		K->class = k->class;
		K->length = sizeof(XKbdFeedbackState);
		K->id = k->id;
		K->click = k->click;
		K->percent = k->percent;
		K->pitch = k->pitch;
		K->duration = k->duration;
		K->led_mask = k->led_mask;
		K->global_auto_repeat = k->global_auto_repeat;
		memcpy((char *)&K->auto_repeats[0],
		       (char *)&k->auto_repeats[0], 32);
		break;
	    }
	    case PtrFeedbackClass:
	    {
		xPtrFeedbackState *p;
		XPtrFeedbackState *P;

		p = (xPtrFeedbackState *) f;
		P = (XPtrFeedbackState *) Feedback;

		P->class = p->class;
		P->length = sizeof(XPtrFeedbackState);
		P->id = p->id;
		P->accelNum = p->accelNum;
		P->accelDenom = p->accelDenom;
		P->threshold = p->threshold;
		break;
	    }
	    case IntegerFeedbackClass:
	    {
		xIntegerFeedbackState *ifs;
		XIntegerFeedbackState *I;

		ifs = (xIntegerFeedbackState *) f;
		I = (XIntegerFeedbackState *) Feedback;

		I->class = ifs->class;
		I->length = sizeof(XIntegerFeedbackState);
		I->id = ifs->id;
		I->resolution = ifs->resolution;
		I->minVal = ifs->min_value;
		I->maxVal = ifs->max_value;
		break;
	    }
	    case StringFeedbackClass:
	    {
		xStringFeedbackState *s;
		XStringFeedbackState *S;

		s = (xStringFeedbackState *) f;
		S = (XStringFeedbackState *) Feedback;

		S->class = s->class;
		S->length = sizeof(XStringFeedbackState) +
		    (s->num_syms_supported * sizeof(KeySym));
		S->id = s->id;
		S->max_symbols = s->max_symbols;
		S->num_syms_supported = s->num_syms_supported;
		S->syms_supported = (KeySym *) (S + 1);
		memcpy((char *)S->syms_supported, (char *)(s + 1),
		       (S->num_syms_supported * sizeof(KeySym)));
		break;
	    }
	    case LedFeedbackClass:
	    {
		xLedFeedbackState *l;
		XLedFeedbackState *L;

		l = (xLedFeedbackState *) f;
		L = (XLedFeedbackState *) Feedback;

		L->class = l->class;
		L->length = sizeof(XLedFeedbackState);
		L->id = l->id;
		L->led_values = l->led_values;
		L->led_mask = l->led_mask;
		break;
	    }
	    case BellFeedbackClass:
	    {
		xBellFeedbackState *b;
		XBellFeedbackState *B;

		b = (xBellFeedbackState *) f;
		B = (XBellFeedbackState *) Feedback;

		B->class = b->class;
		B->length = sizeof(XBellFeedbackState);
		B->id = b->id;
		B->percent = b->percent;
		B->pitch = b->pitch;
		B->duration = b->duration;
		break;
	    }
	    default:
		break;
	    }
	    f = (xFeedbackState *) ((char *)f + f->length);
	    Feedback = (XFeedbackState *) ((char *)Feedback + Feedback->length);
	}
    }
out:
    XFree((char *)sav);

    UnlockDisplay(dpy);
    SyncHandle();
    return (Sav);
}
