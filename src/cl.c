#include "cl.h"

#include <stdio.h>


struct data_cl init_cl(const char *kernel_filename, const char *kernel_entry, size_t max_source_size) {
  cl_platform_id platform_id;
  cl_uint platforms_num;
  cl_device_id device_id;
  cl_uint devices_num;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  cl_kernel kernel;

  clGetPlatformIDs(1, &platform_id, &platforms_num);
  clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &devices_num);
  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
  command_queue = clCreateCommandQueue(context, device_id, 0, NULL);

  FILE *fd;
  const char *filename = kernel_filename;
  size_t source_size;
  char *source_str;

  fd = fopen(filename, "r");
  source_str = (char *) malloc(max_source_size);
  source_size = fread(source_str, 1, max_source_size, fd);
  fclose(fd);

  program = clCreateProgramWithSource(
    context,
    1,
    (const char **) &source_str,
    (const size_t *) &source_size,
    NULL);

  int build_code = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  printf("Program build: %d\n", build_code);
  kernel = clCreateKernel(program, kernel_entry, NULL);

  struct data_cl data;
  data.context = context;
  data.command_queue = command_queue;
  data.kernel = kernel;

  return data;
}
