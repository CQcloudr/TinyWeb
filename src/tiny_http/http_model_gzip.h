/*
 *Author:GeneralSandman
 *Code:https://github.com/GeneralSandman/TinyWeb
 *E-mail:generalsandman@163.com
 *Web:www.dissigil.cn
 */

/*---XXX---
 *
 ****************************************
 *
 */

#ifndef HTTP_MODEL_GZIP_H
#define HTTP_MODEL_GZIP_H


#include <tiny_struct/buffer_t.h>
#include <tiny_struct/chain_t.h>

#include <zlib.h>


typedef struct gzip_config_t
{
    bool enable;
    bool have_buffer;
    unsigned int level;
    unsigned int buffers_4k;
    unsigned int min_len;
    unsigned int wbits;
    unsigned int memlevel;
}gzip_config_t;


typedef struct gzip_context_t
{
    z_stream stream;
    int flush;

    http_chain_t *in;
    http_chain_t *out;

    http_chain_t *curr_in;
    http_chain_t *curr_out;

    http_chain_t *last_in;
    http_chain_t *last_out;

    unsigned int level;
}gzip_context_t;


enum gzip_status
{
    gzip_ok = 0,
    gzip_error,
    gzip_continue,
    gzip_done
};


void get_zip_config(gzip_config_t *conf);

void gzip_context_init(gzip_config_t *conf, gzip_context_t *context);
gzip_status gzip_deflate_init(gzip_context_t *context);
gzip_status gzip_deflate(gzip_context_t *context, http_buffer_t *buffer);
gzip_status gzip_deflate_end(gzip_context_t *context);

gzip_status gzip_body(gzip_context_t *context,
        const std::string &data);

#endif