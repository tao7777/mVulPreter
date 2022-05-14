 GLSurfaceEGLSurfaceControl::GLSurfaceEGLSurfaceControl(
     ANativeWindow* window,
     scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : root_surface_(new SurfaceControl::Surface(window, kRootSurfaceName)),
       gpu_task_runner_(std::move(task_runner)),
       weak_factory_(this) {}
