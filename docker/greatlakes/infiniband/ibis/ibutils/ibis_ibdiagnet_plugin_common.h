/*
 * Copyright (c) 2004-2014 Mellanox Technologies LTD. All rights reserved.
 *
 * This software is available to you under the terms of the
 * OpenIB.org BSD license included below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef IBIS_IBDIAGNET_PLUGIN_COMMON_H_
#define IBIS_IBDIAGNET_PLUGIN_COMMON_H_

//The definitions in this file can't be changed without changing
//ibdiagnet plugin's interface version
typedef void (*pack_data_func_t)(void *data_to_pack,
                                u_int8_t *packed_buffer);
typedef void (*unpack_data_func_t)(void *data_to_unpack,
                                    u_int8_t *unpacked_buffer);
typedef void (*dump_data_func_t)(void *data_to_dump,
                                    FILE * out_port);

typedef struct clbck_data clbck_data_t;
typedef void (*handle_data_func_t)(const clbck_data_t &clbck_data,
                                   int rec_status,
                                   void *p_attribute_data);

typedef struct clbck_data {
    handle_data_func_t m_handle_data_func;
    void*              m_p_obj; //obj containing handle func
    void*              m_data1;
    void*              m_data2;
    void*              m_data3;
} clbck_data_t;

#endif //IBIS_IBDIAGNET_PLUGIN_COMMON_H_
