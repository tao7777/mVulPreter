 Vp9InterpFilter Vp9Parser::ReadInterpFilter() {
   if (reader_.ReadBool())
    return Vp9InterpFilter::INTERP_FILTER_SELECT;
 
   const Vp9InterpFilter table[] = {
      Vp9InterpFilter::EIGHTTAP_SMOOTH, Vp9InterpFilter::EIGHTTAP,
      Vp9InterpFilter::EIGHTTAP_SHARP, Vp9InterpFilter::BILINEAR,
  };
   return table[reader_.ReadLiteral(2)];
 }
