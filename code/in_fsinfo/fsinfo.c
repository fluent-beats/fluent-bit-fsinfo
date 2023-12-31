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


#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_input.h>
#include <fluent-bit/flb_input_plugin.h>
#include <fluent-bit/flb_pack.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/statfs.h>

#include "fsinfo.h"

static int in_fsinfo_collect(struct flb_input_instance *i_ins,
                             struct flb_config *config, void *in_context);

static int in_fsinfo_init(struct flb_input_instance *in,
                      struct flb_config *config, void *data)
{
    int ret;
    struct flb_in_fsinfo_config *ctx;
    const char *pval = NULL;

    /* Initialize context */
    ctx = flb_malloc(sizeof(struct flb_in_fsinfo_config));
    if (!ctx) {
        return -1;
    }
    ctx->ins = in;

    /* mount point */
    pval = flb_input_get_property("mount_point", in);
    if (pval != NULL) {
        ctx->mount_point = flb_strdup(pval);
    } else {
       ctx->mount_point = DEFAULT_MOUNT_POINT;
    }

    /* Collection time setting */
    pval = flb_input_get_property("interval_sec", in);
    if (pval != NULL && atoi(pval) > 0) {
        ctx->interval_sec = atoi(pval);
    }
    else {
        ctx->interval_sec = DEFAULT_INTERVAL_SEC;
    }

    /* Set the context */
    flb_input_set_context(in, ctx);

    /* Set the collector */
    ret = flb_input_set_collector_time(in, in_fsinfo_collect,
                                       ctx->interval_sec,
                                       0, config);
    if (ret == -1) {
        flb_plg_error(ctx->ins, "could not set collector for fs input plugin");
    }

    return 0;
}

static int in_fsinfo_collect(struct flb_input_instance *i_ins,
                             struct flb_config *config, void *in_context)
{
    int entries = 7;
    struct flb_in_fsinfo_config *ctx = in_context;
    struct statfs fs;
    msgpack_packer mp_pck;
    msgpack_sbuffer mp_sbuf;

    /* Collect stats */
    statfs(ctx->mount_point, &fs);

    /* Initialize local msgpack buffer */
    msgpack_sbuffer_init(&mp_sbuf);
    msgpack_packer_init(&mp_pck, &mp_sbuf, msgpack_sbuffer_write);

    /* Pack the data */
    msgpack_pack_array(&mp_pck, 2);
    flb_pack_time_now(&mp_pck);
    msgpack_pack_map(&mp_pck, entries);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_MNT_POINT));
    msgpack_pack_str_body(&mp_pck, STR_KEY_MNT_POINT, strlen(STR_KEY_MNT_POINT));
    msgpack_pack_str(&mp_pck, strlen(ctx->mount_point));
    msgpack_pack_str_body(&mp_pck, ctx->mount_point, strlen(ctx->mount_point));

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_SIZE));
    msgpack_pack_str_body(&mp_pck, STR_KEY_SIZE, strlen(STR_KEY_SIZE));
    msgpack_pack_uint64(&mp_pck, fs.f_bsize);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_BLOCKS));
    msgpack_pack_str_body(&mp_pck, STR_KEY_BLOCKS, strlen(STR_KEY_BLOCKS));
    msgpack_pack_uint64(&mp_pck, fs.f_blocks);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_FREE));
    msgpack_pack_str_body(&mp_pck, STR_KEY_FREE, strlen(STR_KEY_FREE));
    msgpack_pack_uint64(&mp_pck, fs.f_bfree);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_AVAIL));
    msgpack_pack_str_body(&mp_pck, STR_KEY_AVAIL, strlen(STR_KEY_AVAIL));
    msgpack_pack_uint64(&mp_pck, fs.f_bavail);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_FILES));
    msgpack_pack_str_body(&mp_pck, STR_KEY_FILES, strlen(STR_KEY_FILES));
    msgpack_pack_uint64(&mp_pck, fs.f_files);

    msgpack_pack_str(&mp_pck, strlen(STR_KEY_FREE_FILES));
    msgpack_pack_str_body(&mp_pck, STR_KEY_FREE_FILES, strlen(STR_KEY_FREE_FILES));
    msgpack_pack_uint64(&mp_pck, fs.f_ffree);

    flb_input_chunk_append_raw(i_ins, NULL, 0, mp_sbuf.data, mp_sbuf.size);
    msgpack_sbuffer_destroy(&mp_sbuf);
}

static int in_fsinfo_exit(void *data, struct flb_config *config)
{
    struct flb_in_fsinfo_config *ctx = data;

    if (!ctx) {
        return 0;
    }

    /* done */
    flb_free(ctx);

    return 0;
}

struct flb_input_plugin in_fsinfo_plugin = {
    .name         = "fsinfo",
    .description  = "File System Stats",
    .cb_init      = in_fsinfo_init,
    .cb_pre_run   = NULL,
    .cb_collect   = in_fsinfo_collect,
    .cb_flush_buf = NULL,
    .cb_exit      = in_fsinfo_exit
};