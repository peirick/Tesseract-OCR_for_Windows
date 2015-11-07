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

#ifndef JBIG2ENC_JBIG2COMPARATOR_H__
#define JBIG2ENC_JBIG2COMPARATOR_H__

#if defined(sun)
#include <sys/types.h>
#else
#include <stdint.h>
#endif

#include "allheaders.h"

struct Pix;

// -----------------------------------------------------------------------------
// jbig2enc_are_equivalent compares two pix and tell if they are equivalent by
// trying to decide if these symbols are equivalent from visual point of view.
// See http://is.muni.cz/th/208155/fi_m.
//
// It works by looking for accumulations of differences between two templates.
//
// If the difference is bigger than concrete percentage of one of templates
// they are considered different, if such difference doesn't exist than they
// are equivalent.
//
// Parts of this function should be recreated using leptonica functions, which
// should speed up the process, but the principle should remain the same and
// the result as well.
// -----------------------------------------------------------------------------
bool jbig2enc_are_equivalent(PIX *const firstTemplate,
                             PIX *const secondTemplate);

#endif  // JBIG2ENC_JBIG2COMPARATOR_H__
