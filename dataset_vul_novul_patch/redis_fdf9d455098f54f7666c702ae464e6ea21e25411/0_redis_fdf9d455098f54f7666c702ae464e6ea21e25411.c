static void f_parser (lua_State *L, void *ud) {
  int i;
  Proto *tf;
  Closure *cl;
   struct SParser *p = cast(struct SParser *, ud);
   int c = luaZ_lookahead(p->z);
   luaC_checkGC(L);
  tf = (luaY_parser)(L, p->z,
                                                              &p->buff, p->name);
   cl = luaF_newLclosure(L, tf->nups, hvalue(gt(L)));
   cl->l.p = tf;
  for (i = 0; i < tf->nups; i++)  /* initialize eventual upvalues */
    cl->l.upvals[i] = luaF_newupval(L);
  setclvalue(L, L->top, cl);
  incr_top(L);
}
