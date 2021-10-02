#pragma once

#define CL_TARGET_OPENCL_VERSION 300
#include "CL/cl.h"


struct data_cl {
  cl_context context;
  cl_kernel kernel;
  cl_command_queue command_queue;
};

struct data_cl init_cl(const char *kernel_filename, const char *kernel_entry, size_t max_source_size);
