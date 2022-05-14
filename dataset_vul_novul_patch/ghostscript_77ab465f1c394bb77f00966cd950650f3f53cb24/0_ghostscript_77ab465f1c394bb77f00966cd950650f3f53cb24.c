static void jsR_calllwfunction(js_State *J, int n, js_Function *F, js_Environment *scope)
{
	js_Value v;
	int i;

        jsR_savescope(J, scope);
 
        if (n > F->numparams) {
               js_pop(J, n - F->numparams);
                n = F->numparams;
        }
        for (i = n; i < F->varlen; ++i)
		js_pushundefined(J);

	jsR_run(J, F);
	v = *stackidx(J, -1);
	TOP = --BOT; /* clear stack */
	js_pushvalue(J, v);

	jsR_restorescope(J);
}
