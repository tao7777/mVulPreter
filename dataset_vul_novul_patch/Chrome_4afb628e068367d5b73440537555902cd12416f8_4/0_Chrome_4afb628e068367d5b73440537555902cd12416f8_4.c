 GLSurfaceEGLSurfaceControl::GLSurfaceEGLSurfaceControl(
     ANativeWindow* window,
     scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : window_rect_(0,
                   0,
                   ANativeWindow_getWidth(window),
                   ANativeWindow_getHeight(window)),
      root_surface_(new SurfaceControl::Surface(window, kRootSurfaceName)),
       gpu_task_runner_(std::move(task_runner)),
       weak_factory_(this) {}
