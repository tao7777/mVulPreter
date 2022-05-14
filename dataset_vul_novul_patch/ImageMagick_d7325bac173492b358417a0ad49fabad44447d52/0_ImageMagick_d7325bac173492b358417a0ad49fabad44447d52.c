static void ComputePrincipleComponent(const float *covariance,
  DDSVector3 *principle)
{
  DDSVector4
    row0,
    row1,
    row2,
    v;

  register ssize_t
    i;

  row0.x = covariance[0];
  row0.y = covariance[1];
  row0.z = covariance[2];
  row0.w = 0.0f;

  row1.x = covariance[1];
  row1.y = covariance[3];
  row1.z = covariance[4];
  row1.w = 0.0f;

  row2.x = covariance[2];
  row2.y = covariance[4];
  row2.z = covariance[5];
  row2.w = 0.0f;

  VectorInit(v,1.0f);

  for (i=0; i < 8; i++)
  {
    DDSVector4
      w;

    float
      a;

    w.x = row0.x * v.x;
    w.y = row0.y * v.x;
    w.z = row0.z * v.x;
    w.w = row0.w * v.x;

    w.x = (row1.x * v.y) + w.x;
    w.y = (row1.y * v.y) + w.y;
    w.z = (row1.z * v.y) + w.z;
    w.w = (row1.w * v.y) + w.w;

    w.x = (row2.x * v.z) + w.x;
    w.y = (row2.y * v.z) + w.y;
     w.z = (row2.z * v.z) + w.z;
     w.w = (row2.w * v.z) + w.w;
 
    a = 1.0f / MagickMax(w.x,MagickMax(w.y,w.z));
 
     v.x = w.x * a;
     v.y = w.y * a;
    v.z = w.z * a;
    v.w = w.w * a;
  }

  VectorCopy43(v,principle);
}
