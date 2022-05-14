bool SubsetterImpl::ResolveCompositeGlyphs(const unsigned int* glyph_ids,
                                           size_t glyph_count,
                                           IntegerSet* glyph_id_processed) {
  if (glyph_ids == NULL || glyph_count == 0 || glyph_id_processed == NULL) {
    return false;
  }

  GlyphTablePtr glyph_table =
      down_cast<GlyphTable*>(font_->GetTable(Tag::glyf));
  LocaTablePtr loca_table = down_cast<LocaTable*>(font_->GetTable(Tag::loca));
  if (glyph_table == NULL || loca_table == NULL) {
    return false;
  }

  IntegerSet glyph_id_remaining;
  glyph_id_remaining.insert(0);  // Always include glyph id 0.
  for (size_t i = 0; i < glyph_count; ++i) {
    glyph_id_remaining.insert(glyph_ids[i]);
  }

  while (!glyph_id_remaining.empty()) {
    IntegerSet comp_glyph_id;
    for (IntegerSet::iterator i = glyph_id_remaining.begin(),
                              e = glyph_id_remaining.end(); i != e; ++i) {
      if (*i < 0 || *i >= loca_table->NumGlyphs()) {
        continue;
      }

      int32_t length = loca_table->GlyphLength(*i);
      if (length == 0) {
        continue;
      }
      int32_t offset = loca_table->GlyphOffset(*i);

      GlyphPtr glyph;
      glyph.Attach(glyph_table->GetGlyph(offset, length));
      if (glyph == NULL) {
        continue;
      }

      if (glyph->GlyphType() == GlyphType::kComposite) {
        Ptr<GlyphTable::CompositeGlyph> comp_glyph =
            down_cast<GlyphTable::CompositeGlyph*>(glyph.p_);
        for (int32_t j = 0; j < comp_glyph->NumGlyphs(); ++j) {
          int32_t glyph_id = comp_glyph->GlyphIndex(j);
          if (glyph_id_processed->find(glyph_id) == glyph_id_processed->end() &&
              glyph_id_remaining.find(glyph_id) == glyph_id_remaining.end()) {
            comp_glyph_id.insert(comp_glyph->GlyphIndex(j));
          }
        }
      }

      glyph_id_processed->insert(*i);
    }

     glyph_id_remaining.clear();
     glyph_id_remaining = comp_glyph_id;
   }

  return true;
 }
