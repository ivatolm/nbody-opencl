__kernel void compute(__global float *positions,
                      __global float *velocities,
                      float dt,
                      int n,
                      __global float *new_positions)
{
  const float G = 1e-5, mass = 100, radius = 1;
  int idx = get_global_id(0);
  const int X = 2 * idx + 0, Y = 2 * idx + 1;

  float posX = positions[X], posY = positions[Y];
  float velX = velocities[X], velY = velocities[Y];
  float accX = 0.0f, accY = 0.0f;
  for (int i = 0; i < n; i++) {
    const int x = 2 * i + 0, y = 2 * i + 1;

    float o_posX = positions[x], o_posY = positions[y];
    float o_velX = velocities[x], o_velY = velocities[y];
    float distX = o_posX - posX, distY = o_posY - posY;
    float dist = sqrt(distX * distX + distY * distY);

    float dirX = distX / dist, dirY = distY / dist;

    if (dist == 0) continue;
    float acc = G * mass / dist;
    accX += acc * dirX, accY += acc * dirY;
  }

  velX += accX * dt, velY += accY * dt;
  posX += velX, posY += velY;

  velocities[X] = velX, velocities[Y] = velY;
  new_positions[X] = posX, new_positions[Y] = posY;
}
