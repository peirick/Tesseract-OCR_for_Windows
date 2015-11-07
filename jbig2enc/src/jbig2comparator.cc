// Copyright 2012 Google Inc. All Rights Reserved.
// Author: hata.radim@gmail.com (Radim Hatlapatka)
//
// Copyright (C) 2012 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <map>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include "allheaders.h"

#include <math.h>
#if defined(sun)
#include <sys/types.h>
#else
#include <stdint.h>
#endif

#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t

bool
jbig2enc_are_equivalent(PIX *const first_template, PIX *const second_template) {
  l_int32 w, h, d;

  if (!pixSizesEqual(first_template, second_template)) {
    return false;
  }

  l_int32 first_wpl = pixGetWpl(first_template);
  l_int32 second_wpl = pixGetWpl(second_template);

  if (first_wpl != second_wpl) {
    return false;
  }

  PIX *pixd = pixXor(NULL, first_template, second_template);

  pixGetDimensions(pixd, &w, &h, &d);

  l_int32 init = 0;
  l_int32 *pcount = &init;
  l_int32 *above = &init;

  // counting number of ON pixels in first_template
  if (pixCountPixels(first_template, pcount, NULL)) {
    fprintf(stderr, "Unable to count pixels\n");
    pixDestroy(&pixd);
    return false;
  }

  // shortcut to failure if the symbols are significantly different.
  l_int32 thresh = (*pcount) * 0.25;
  if (pixThresholdPixelSum(pixd, thresh, above, NULL)) {
    fprintf(stderr, "Unable to count pixels of XORed pixes\n");
    pixDestroy(&pixd);
    return false;
  }

  if ((*above) == 1) {
    pixDestroy(&pixd);
    return false;
  }

  l_uint32 init_unsigned = 0;
  l_uint32 *pval = &init_unsigned;
  const int divider = 9;
  const int vertical = divider * 2;
  const int horizontal = divider * 2;

  l_uint32 parsed_pix_counts[divider][divider];
  l_uint32 horizontal_parsed_pix_counts[horizontal][divider];
  l_uint32 vertical_parsed_pix_counts[divider][vertical];

  if (d != 1) {
    return false;
  }

  int vertical_part = h/divider;
  int horizontal_part = w/divider;

  int horizontal_module_counter = 0;
  int vertical_module_counter = 0;

  // counting area of ellipse and taking percentage of it as point_thresh
  int a, b;
  if (vertical_part < horizontal_part) {
    a = horizontal_part / 2;
    b = vertical_part / 2;
  } else {
    a = vertical_part / 2;
    b = horizontal_part / 2;
  }

  float point_thresh = a * b * M_PI;
  l_int32 vline_thresh = (vertical_part * (horizontal_part/2))*0.9;
  l_int32 hline_thresh = (horizontal_part * (vertical_part/2))*0.9;

  // iterate through submatrixes
  for (int horizontal_position = 0; horizontal_position < divider; horizontal_position++) {
    int horizontal_start = horizontal_part*horizontal_position + horizontal_module_counter;
    int horizontal_end;
    if (horizontal_position == (divider-1)) {
      horizontal_module_counter = 0;
      horizontal_end = w;
    } else {
      if (((w - horizontal_module_counter) % divider)>0) {
        horizontal_end = horizontal_start + horizontal_part + 1;
        horizontal_module_counter++;
      } else {
        horizontal_end = horizontal_start + horizontal_part;
      }
    }

    for (int vertical_position = 0; vertical_position < divider; vertical_position++) {
      int vertical_start = vertical_part*vertical_position + vertical_module_counter;
      int vertical_end;
      if (vertical_position == (divider-1)) {
        vertical_module_counter = 0;
        vertical_end = h;
      } else {
        if (((h - vertical_module_counter) % divider)>0) {
          vertical_end = vertical_start + vertical_part + 1;
          vertical_module_counter++;
        } else {
          vertical_end = vertical_start + vertical_part;
        }
      }

      // making sum of ON pixels in submatrix and saving the result to matrix of sums.
      int left_count = 0;
      int right_count = 0;
      int down_count = 0;
      int up_count = 0;

      int horizontal_center = (horizontal_start + horizontal_end) / 2;
      int vertical_center = (vertical_start + vertical_end) / 2;

      for (int i = horizontal_start; i < horizontal_end; i++) {
        for (int j = vertical_start; j < vertical_end; j++) {
          if (pixGetPixel(pixd, i, j, pval)) {
            fprintf(stderr, "unable to read pixel from pix\n");
            break;
          }

          if (*pval == 1) {
            if (i < horizontal_center) {
              left_count++;
            } else {
              right_count++;
            }
            if (j < vertical_center) {
              up_count++;
            } else {
              down_count++;
            }
          }
        }
      }
      parsed_pix_counts[horizontal_position][vertical_position] = left_count + right_count;

      horizontal_parsed_pix_counts[horizontal_position*2][vertical_position] = left_count;
      horizontal_parsed_pix_counts[(horizontal_position*2)+1][vertical_position] = right_count;

      vertical_parsed_pix_counts[horizontal_position][vertical_position*2] = up_count;
      vertical_parsed_pix_counts[horizontal_position][(vertical_position*2)+1] = down_count;
    }
  }

  pixDestroy(&pixd);

  // check for horizontal lines
  for (int i = 0; (i < (divider*2)-1); i++) {
    for (int j = 0; j < (divider-1); j++) {
      int horizontal_sum = 0;
      for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
          horizontal_sum += horizontal_parsed_pix_counts[i+x][j+y];
        }
      }
      if (horizontal_sum > hline_thresh) {
        return 0;
      }
    }
  }

  // check for vertical lines
  for (int i = 0; i < (divider-1); i++) {
    for (int j = 0; j < ((divider*2)-1); j++) {
      int vertical_sum = 0;
      for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
        vertical_sum += vertical_parsed_pix_counts[i+x][j+y];
        }
      }
      if (vertical_sum > vline_thresh) {
        return 0;
      }
    }
  }

  // check for cross lines
  for (int i = 0; i < (divider - 2); i++) {
    for (int j = 0; j < (divider - 2); j++) {
      int left_cross = 0;
      int right_cross = 0;
      for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
          if (x == y) {
            left_cross += parsed_pix_counts[i+x][j+y];
          }
          if ((2-x) == y) {
            right_cross += parsed_pix_counts[i+x][j+y];
          }
        }
      }
      if ((left_cross > hline_thresh) || (right_cross > hline_thresh)) {
        return 0;
      }
    }
  }

  // check whether four submatrixes of XORed PIX data contains more ON pixels
  // than concrete percentage of ON pixels of first_template.

  for (int i = 0; i < (divider-1); i++) {
    for (int j = 0; j < (divider-1); j++) {
      int sum = 0;
      for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
          sum += parsed_pix_counts[i+x][j+y];
        }
      }
      if (sum > point_thresh) {
        return 0;
      }
    }
  }
  return 1;
}
