void LayerTilerChromium::resizeLayer(const IntSize& size)
{
    if (m_layerSize == size)
        return;

     int width = (size.width() + m_tileSize.width() - 1) / m_tileSize.width();
     int height = (size.height() + m_tileSize.height() - 1) / m_tileSize.height();
 
    if (height && (width > INT_MAX / height))
        CRASH();

     Vector<OwnPtr<Tile> > newTiles;
     newTiles.resize(width * height);
     for (int j = 0; j < m_layerTileSize.height(); ++j)
        for (int i = 0; i < m_layerTileSize.width(); ++i)
            newTiles[i + j * width].swap(m_tiles[i + j * m_layerTileSize.width()]);

    m_tiles.swap(newTiles);
    m_layerSize = size;
    m_layerTileSize = IntSize(width, height);
}
