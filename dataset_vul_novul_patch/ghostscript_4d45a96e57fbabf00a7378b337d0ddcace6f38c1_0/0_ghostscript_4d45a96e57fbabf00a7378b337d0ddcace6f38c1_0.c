 static js_Ast *memberexp(js_State *J)
 {
       js_Ast *a = newexp(J);
       SAVEREC();
 loop:
       INCREC();
        if (jsP_accept(J, '.')) { a = EXP2(MEMBER, a, identifiername(J)); goto loop; }
        if (jsP_accept(J, '[')) { a = EXP2(INDEX, a, expression(J, 0)); jsP_expect(J, ']'); goto loop; }
       POPREC();
        return a;
 }
