 void ParamTraits<GURL>::Write(Message* m, const GURL& p) {
   DCHECK(p.possibly_invalid_spec().length() <= content::kMaxURLChars);
   m->WriteString(p.possibly_invalid_spec());
 }
