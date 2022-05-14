bool CanvasSurfaceLayerBridge::createSurfaceLayer(int canvasWidth, int canvasHeight)
{
     if (!m_client->syncGetSurfaceId(&m_surfaceId))
         return false;
 
     cc::SurfaceLayer::SatisfyCallback satisfyCallback = convertToBaseCallback(WTF::bind(&CanvasSurfaceLayerBridge::satisfyCallback, WTF::unretained(this)));
     cc::SurfaceLayer::RequireCallback requireCallback = convertToBaseCallback(WTF::bind(&CanvasSurfaceLayerBridge::requireCallback, WTF::unretained(this)));
     m_surfaceLayer = cc::SurfaceLayer::Create(std::move(satisfyCallback), std::move(requireCallback));
    m_surfaceLayer->SetSurfaceId(m_surfaceId, 1.f, gfx::Size(canvasWidth, canvasHeight));

    m_webLayer = wrapUnique(Platform::current()->compositorSupport()->createLayerFromCCLayer(m_surfaceLayer.get()));
    GraphicsLayer::registerContentsLayer(m_webLayer.get());
    return true;
}
