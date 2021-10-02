#include "cl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>


int main() {
  struct data_cl data_cl = init_cl("src/kernel.cl", "compute", 65536);

  const int n = 100000, w = 1600, h = 900;

  float positions[2 * n], velocities[2 * n], new_positions[2 * n];
  float dt = 1;
  for (int i = 0; i < n; i++) {
    positions[2 * i + 0] = rand() % w;
    positions[2 * i + 1] = rand() % h;
    new_positions[2 * i + 0] = rand() % (w / 8) + (w / 2.f);
    new_positions[2 * i + 1] = rand() % (h / 8) + (h / 2.f);
    velocities[2 * i + 0] = ((rand() % 2) ? -1 : 1) * (rand() % 100) / 100.0f;
    velocities[2 * i + 1] = ((rand() % 2) ? -1 : 1) * (rand() % 100) / 100.0f;
  }

  cl_mem mem_pos = clCreateBuffer(data_cl.context, CL_MEM_READ_WRITE, 2 * n * sizeof(float), NULL, NULL);
  cl_mem mem_vel = clCreateBuffer(data_cl.context, CL_MEM_READ_WRITE, 2 * n * sizeof(float), NULL, NULL);
  cl_mem mem_npos = clCreateBuffer(data_cl.context, CL_MEM_READ_WRITE, 2 * n * sizeof(float), NULL, NULL);

  clEnqueueWriteBuffer(data_cl.command_queue, mem_pos, CL_TRUE, 0, 2 * n * sizeof(float), positions, 0, NULL, NULL);
  clEnqueueWriteBuffer(data_cl.command_queue, mem_vel, CL_TRUE, 0, 2 * n * sizeof(float), velocities, 0, NULL, NULL);
  clEnqueueWriteBuffer(data_cl.command_queue, mem_npos, CL_TRUE, 0, 2 * n * sizeof(float), new_positions, 0, NULL, NULL);

  clSetKernelArg(data_cl.kernel, 0, sizeof(cl_mem), (void *) &mem_pos);
  clSetKernelArg(data_cl.kernel, 1, sizeof(cl_mem), (void *) &mem_vel);
  clSetKernelArg(data_cl.kernel, 2, sizeof(float), &dt);
  clSetKernelArg(data_cl.kernel, 3, sizeof(int), &n);
  clSetKernelArg(data_cl.kernel, 4, sizeof(cl_mem), (void *) &mem_npos);

  sf::RenderWindow window(sf::VideoMode(w, h), "Simulation");

  while (1) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        goto l_exit;
    }

    size_t global_work_size[] = { n };
    int run_code = clEnqueueNDRangeKernel(data_cl.command_queue, data_cl.kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(data_cl.command_queue, mem_pos, CL_TRUE, 0, 2 * n * sizeof(float), positions, 0, NULL, NULL);
    clEnqueueReadBuffer(data_cl.command_queue, mem_vel, CL_TRUE, 0, 2 * n * sizeof(float), velocities, 0, NULL, NULL);
    clEnqueueReadBuffer(data_cl.command_queue, mem_npos, CL_TRUE, 0, 2 * n * sizeof(float), new_positions, 0, NULL, NULL);

    for (int i = 0; i < n; i++) {
      positions[2 * i + 0] = new_positions[2 * i + 0];
      positions[2 * i + 1] = new_positions[2 * i + 1];
    }
    clEnqueueWriteBuffer(data_cl.command_queue, mem_pos, CL_TRUE, 0, 2 * n * sizeof(float), positions, 0, NULL, NULL);

    window.clear();
    for (int i = 0; i < n; i++) {
      sf::CircleShape shape(0.5f);
      shape.setPosition(positions[2 * i + 0], positions[2 * i + 1]);
      window.draw(shape);
    }
    window.display();
  }
l_exit:
  window.close();
}
