 void ManifestChangeNotifier::DidChangeManifest() {
   if (weak_factory_.HasWeakPtrs())
     return;
 
  if (!render_frame()->GetWebFrame()->IsLoading()) {
    render_frame()
        ->GetTaskRunner(blink::TaskType::kUnspecedLoading)
        ->PostTask(FROM_HERE,
                   base::BindOnce(&ManifestChangeNotifier::ReportManifestChange,
                                  weak_factory_.GetWeakPtr()));
    return;
  }
  ReportManifestChange();
}
