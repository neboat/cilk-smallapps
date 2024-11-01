//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2010-2013 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// ===============================================================

// Each of these methods calculate how deeply numbers on a complex plane remains
// in the Mandelbrot set. On top of the serial/scalar version, there is a
// cilk_for version, a pragma simd version, and a combined cilk_for/pragma simd
// version

#include "mandelbrot.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cstdlib>

// Description:
// Determines how deeply points in the complex plane, spaced on a uniform grid,
// remain in the Mandelbrot set. The uniform grid is specified by the rectangle
// (x1, y1) - (x0, y0). Mandelbrot set is determined by remaining bounded after
// iteration of z_n+1 = z_n^2 + c, up to max_depth.
//
// iterating through the complex plane is accomplished with cilk_for
//
// [in]: x0, y0, x1, y1, width, height, max_depth
// [out]: output (caller must deallocate)
unsigned char *cilk_mandelbrot(double x0, double y0, double x1, double y1,
                               int width, int height, int max_depth) {
  double xstep = (x1 - x0) / width;
  double ystep = (y1 - y0) / height;
  unsigned char *output = static_cast<unsigned char *>(
      aligned_alloc(64, width * height * sizeof(unsigned char)));
  // Traverse the sample space in equally spaced steps with width * height
  // samples
  cilk_for(int j = 0; j < height; ++j) {
    cilk_for(int i = 0; i < width; ++i) {
      double z_real = x0 + i * xstep;
      double z_imaginary = y0 + j * ystep;
      double c_real = z_real;
      double c_imaginary = z_imaginary;

      int depth = 0;
      // Figures out how many recurrences are required before divergence, up to
      // max_depth
      while (depth < max_depth) {
        if (z_real * z_real + z_imaginary * z_imaginary > 4.0) {
          break; // Escape from a circle of radius 2
        }
        double temp_real = z_real * z_real - z_imaginary * z_imaginary;
        double temp_imaginary = 2.0 * z_real * z_imaginary;
        z_real = c_real + temp_real;
        z_imaginary = c_imaginary + temp_imaginary;

        ++depth;
      }
      output[j * width + i] = static_cast<unsigned char>(
          static_cast<double>(depth) / max_depth * 255);
    }
  }
  return output;
}
