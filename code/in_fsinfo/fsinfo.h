/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Etriphany
 *  ==========
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef FLB_IN_FSINFO_H
#define FLB_IN_FSINFO_H

#include <fluent-bit/flb_input.h>
#include <fluent-bit/flb_config.h>
#include <fluent-bit/flb_utils.h>

#define DEFAULT_INTERVAL_SEC  10
#define DEFAULT_MOUNT_POINT "/"


#define STR_KEY_MNT_POINT   "mnt"
#define STR_KEY_SIZE        "f_bsize"
#define STR_KEY_BLOCKS      "f_blocks"
#define STR_KEY_FREE        "f_bfree"
#define STR_KEY_AVAIL       "f_bavail"
#define STR_KEY_FILES       "f_files"
#define STR_KEY_FREE_FILES  "f_ffree"

struct flb_in_fsinfo_config {
    char*  mount_point;
    int    interval_sec;
    struct flb_input_instance *ins;
};

#endif