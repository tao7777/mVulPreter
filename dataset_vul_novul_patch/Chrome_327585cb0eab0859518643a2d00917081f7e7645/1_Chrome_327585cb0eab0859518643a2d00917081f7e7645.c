WebGraphicsContext3DDefaultImpl::WebGraphicsContext3DDefaultImpl()
    : m_initialized(false)
    , m_renderDirectlyToWebView(false)
     , m_texture(0)
     , m_fbo(0)
     , m_depthStencilBuffer(0)
     , m_multisampleFBO(0)
     , m_multisampleDepthStencilBuffer(0)
     , m_multisampleColorBuffer(0)
    , m_boundFBO(0)
#ifdef FLIP_FRAMEBUFFER_VERTICALLY
    , m_scanline(0)
#endif
    , m_boundArrayBuffer(0)
    , m_fragmentCompiler(0)
    , m_vertexCompiler(0)
{
}
