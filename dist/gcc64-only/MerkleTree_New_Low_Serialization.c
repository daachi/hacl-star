/* MIT License
 *
 * Copyright (c) 2016-2020 INRIA, CMU and Microsoft Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "MerkleTree_New_Low_Serialization.h"

typedef struct __bool_uint32_t_s
{
  bool fst;
  uint32_t snd;
}
__bool_uint32_t;

static __bool_uint32_t
serialize_bool(bool ok, bool x, uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  uint8_t ite;
  if (x)
  {
    ite = (uint8_t)1U;
  }
  else
  {
    ite = (uint8_t)0U;
  }
  buf1[pos] = ite;
  return ((__bool_uint32_t){ .fst = true, .snd = pos + (uint32_t)1U });
}

static __bool_uint32_t
serialize_uint8_t(bool ok, uint8_t x, uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  buf1[pos] = x;
  return ((__bool_uint32_t){ .fst = true, .snd = pos + (uint32_t)1U });
}

static __bool_uint32_t
serialize_uint16_t(bool ok, uint16_t x, uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  __bool_uint32_t scrut = serialize_uint8_t(ok, (uint8_t)(x >> (uint32_t)8U), buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  return serialize_uint8_t(ok1, (uint8_t)x, buf1, sz, pos1);
}

static __bool_uint32_t
serialize_uint32_t(bool ok, uint32_t x, uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  __bool_uint32_t scrut = serialize_uint16_t(ok, (uint16_t)(x >> (uint32_t)16U), buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  return serialize_uint16_t(ok1, (uint16_t)x, buf1, sz, pos1);
}

static __bool_uint32_t
serialize_uint64_t(bool ok, uint64_t x, uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  __bool_uint32_t scrut = serialize_uint32_t(ok, (uint32_t)(x >> (uint32_t)32U), buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  return serialize_uint32_t(ok1, (uint32_t)x, buf1, sz, pos1);
}

static __bool_uint32_t
(*serialize_offset_t)(bool x0, uint64_t x1, uint8_t *x2, uint32_t x3, uint32_t x4) =
  serialize_uint64_t;

static __bool_uint32_t
serialize_hash_i(
  uint32_t hash_size,
  bool ok,
  uint8_t *x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos,
  uint32_t i1
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  uint8_t b = x[i1];
  __bool_uint32_t scrut = serialize_uint8_t(ok, b, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  uint32_t j1 = i1 + (uint32_t)1U;
  if (j1 < hash_size)
  {
    return serialize_hash_i(hash_size, ok1, x, buf1, sz, pos1, j1);
  }
  return ((__bool_uint32_t){ .fst = ok1, .snd = pos1 });
}

static __bool_uint32_t
serialize_hash(
  uint32_t hash_size,
  bool ok,
  uint8_t *x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  return serialize_hash_i(hash_size, ok, x, buf1, sz, pos, (uint32_t)0U);
}

static __bool_uint32_t
serialize_hash_vec_i(
  uint32_t hash_size,
  bool ok,
  LowStar_Vector_vector_str___uint8_t_ x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos,
  uint32_t i1
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  uint8_t *vi = LowStar_Vector_index___uint8_t_(x, i1);
  __bool_uint32_t scrut = serialize_hash(hash_size, ok, vi, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  uint32_t j1 = i1 + (uint32_t)1U;
  if (j1 < x.sz)
  {
    return serialize_hash_vec_i(hash_size, ok1, x, buf1, sz, pos1, j1);
  }
  return ((__bool_uint32_t){ .fst = ok1, .snd = pos1 });
}

static __bool_uint32_t
serialize_hash_vec(
  uint32_t hash_size,
  bool ok,
  LowStar_Vector_vector_str___uint8_t_ x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  __bool_uint32_t scrut = serialize_uint32_t(ok, x.sz, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  if (ok1 && x.sz > (uint32_t)0U)
  {
    return serialize_hash_vec_i(hash_size, ok1, x, buf1, sz, pos1, (uint32_t)0U);
  }
  return ((__bool_uint32_t){ .fst = ok1, .snd = pos1 });
}

static uint64_t
hash_vv_bytes_i(
  uint32_t hash_size,
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ vv1,
  uint32_t i1
)
{
  if (i1 >= vv1.sz)
  {
    return (uint64_t)4U;
  }
  LowStar_Vector_vector_str___uint8_t_
  vvi = LowStar_Vector_index__LowStar_Vector_vector_str__uint8_t_(vv1, i1);
  uint64_t vs_hs = (uint64_t)vvi.sz * (uint64_t)hash_size;
  uint64_t r;
  if (uint64_max - vs_hs >= (uint64_t)4U)
  {
    r = vs_hs + (uint64_t)4U;
  }
  else
  {
    r = uint64_max;
  }
  uint64_t rest = hash_vv_bytes_i(hash_size, vv1, i1 + (uint32_t)1U);
  if (uint64_max - r >= rest)
  {
    return r + rest;
  }
  return uint64_max;
}

static __bool_uint32_t
serialize_hash_vv_i(
  uint32_t hash_size,
  bool ok,
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos,
  uint32_t i1
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  LowStar_Vector_vector_str___uint8_t_
  vi = LowStar_Vector_index__LowStar_Vector_vector_str__uint8_t_(x, i1);
  __bool_uint32_t scrut = serialize_hash_vec(hash_size, ok, vi, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  uint32_t j1 = i1 + (uint32_t)1U;
  if (j1 < x.sz)
  {
    return serialize_hash_vv_i(hash_size, ok1, x, buf1, sz, pos1, j1);
  }
  return ((__bool_uint32_t){ .fst = ok1, .snd = pos1 });
}

static __bool_uint32_t
serialize_hash_vv(
  uint32_t hash_size,
  bool ok,
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ x,
  uint8_t *buf1,
  uint32_t sz,
  uint32_t pos
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  __bool_uint32_t scrut = serialize_uint32_t(ok, x.sz, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  if (x.sz > (uint32_t)0U)
  {
    return serialize_hash_vv_i(hash_size, ok1, x, buf1, sz, pos1, (uint32_t)0U);
  }
  return ((__bool_uint32_t){ .fst = ok1, .snd = pos1 });
}

typedef struct __bool_uint32_t_bool_s
{
  bool fst;
  uint32_t snd;
  bool thd;
}
__bool_uint32_t_bool;

static __bool_uint32_t_bool
deserialize_bool(bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t_bool){ .fst = false, .snd = pos, .thd = false });
  }
  bool sw;
  switch (buf1[pos])
  {
    case 0U:
      {
        sw = false;
        break;
      }
    default:
      {
        sw = true;
      }
  }
  return ((__bool_uint32_t_bool){ .fst = true, .snd = pos + (uint32_t)1U, .thd = sw });
}

typedef struct __bool_uint32_t_uint8_t_s
{
  bool fst;
  uint32_t snd;
  uint8_t thd;
}
__bool_uint32_t_uint8_t;

static __bool_uint32_t_uint8_t
deserialize_uint8_t(bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t_uint8_t){ .fst = false, .snd = pos, .thd = (uint8_t)0U });
  }
  return ((__bool_uint32_t_uint8_t){ .fst = true, .snd = pos + (uint32_t)1U, .thd = buf1[pos] });
}

typedef struct __bool_uint32_t_uint16_t_s
{
  bool fst;
  uint32_t snd;
  uint16_t thd;
}
__bool_uint32_t_uint16_t;

static __bool_uint32_t_uint16_t
deserialize_uint16_t(bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t_uint16_t){ .fst = false, .snd = pos, .thd = (uint16_t)0U });
  }
  __bool_uint32_t_uint8_t scrut0 = deserialize_uint8_t(ok, buf1, sz, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  uint8_t b0 = scrut0.thd;
  __bool_uint32_t_uint8_t scrut = deserialize_uint8_t(ok1, buf1, sz, pos1);
  bool ok2 = scrut.fst;
  uint32_t pos2 = scrut.snd;
  uint8_t b1 = scrut.thd;
  return
    (
      (__bool_uint32_t_uint16_t){
        .fst = ok2,
        .snd = pos2,
        .thd = ((uint16_t)b0 << (uint32_t)8U) + (uint16_t)b1
      }
    );
}

typedef struct __bool_uint32_t_uint32_t_s
{
  bool fst;
  uint32_t snd;
  uint32_t thd;
}
__bool_uint32_t_uint32_t;

static __bool_uint32_t_uint32_t
deserialize_uint32_t(bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t_uint32_t){ .fst = false, .snd = pos, .thd = (uint32_t)0U });
  }
  __bool_uint32_t_uint16_t scrut0 = deserialize_uint16_t(ok, buf1, sz, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  uint16_t b0 = scrut0.thd;
  __bool_uint32_t_uint16_t scrut = deserialize_uint16_t(ok1, buf1, sz, pos1);
  bool ok2 = scrut.fst;
  uint32_t pos2 = scrut.snd;
  uint16_t b1 = scrut.thd;
  return
    (
      (__bool_uint32_t_uint32_t){
        .fst = ok2,
        .snd = pos2,
        .thd = ((uint32_t)b0 << (uint32_t)16U) + (uint32_t)b1
      }
    );
}

typedef struct __bool_uint32_t_uint64_t_s
{
  bool fst;
  uint32_t snd;
  uint64_t thd;
}
__bool_uint32_t_uint64_t;

static __bool_uint32_t_uint64_t
deserialize_uint64_t(bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t_uint64_t){ .fst = false, .snd = pos, .thd = (uint64_t)0U });
  }
  __bool_uint32_t_uint32_t scrut0 = deserialize_uint32_t(ok, buf1, sz, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  uint32_t b0 = scrut0.thd;
  __bool_uint32_t_uint32_t scrut = deserialize_uint32_t(ok1, buf1, sz, pos1);
  bool ok2 = scrut.fst;
  uint32_t pos2 = scrut.snd;
  uint32_t b1 = scrut.thd;
  return
    (
      (__bool_uint32_t_uint64_t){
        .fst = ok2,
        .snd = pos2,
        .thd = ((uint64_t)b0 << (uint32_t)32U) + (uint64_t)b1
      }
    );
}

static __bool_uint32_t_uint64_t
(*deserialize_offset_t)(bool x0, const uint8_t *x1, uint32_t x2, uint32_t x3) =
  deserialize_uint64_t;

static __bool_uint32_t_uint32_t
(*deserialize_index_t)(bool x0, const uint8_t *x1, uint32_t x2, uint32_t x3) =
  deserialize_uint32_t;

typedef struct __bool_uint32_t__uint8_t__s
{
  bool fst;
  uint32_t snd;
  uint8_t *thd;
}
__bool_uint32_t__uint8_t_;

static __bool_uint32_t__uint8_t_
deserialize_hash(uint32_t hash_size, bool ok, const uint8_t *buf1, uint32_t sz, uint32_t pos)
{
  LowStar_Regional_regional__uint32_t__uint8_t_
  rg =
    { .state = hash_size, .dummy = hash_dummy, .r_alloc = hash_r_alloc, .r_free = hash_r_free };
  if (!ok || pos >= sz)
  {
    return
      (
        (__bool_uint32_t__uint8_t_){
          .fst = false,
          .snd = pos,
          .thd = LowStar_Regional_rg_dummy___uint8_t__uint32_t(rg)
        }
      );
  }
  if (sz - pos < hash_size)
  {
    return
      (
        (__bool_uint32_t__uint8_t_){
          .fst = false,
          .snd = pos,
          .thd = LowStar_Regional_rg_dummy___uint8_t__uint32_t(rg)
        }
      );
  }
  uint8_t *hash1 = LowStar_Regional_rg_alloc___uint8_t__uint32_t(rg);
  memcpy(hash1, (uint8_t *)buf1 + pos, hash_size * sizeof (((uint8_t *)buf1)[0U]));
  return ((__bool_uint32_t__uint8_t_){ .fst = true, .snd = pos + hash_size, .thd = hash1 });
}

static __bool_uint32_t
deserialize_hash_vec_i(
  uint32_t hash_size,
  bool ok,
  const uint8_t *buf1,
  uint32_t sz,
  uint32_t pos,
  LowStar_Vector_vector_str___uint8_t_ res,
  uint32_t i1
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = pos });
  }
  __bool_uint32_t__uint8_t_ scrut = deserialize_hash(hash_size, ok, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  uint8_t *h1 = scrut.thd;
  if (!ok1)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = pos1 });
  }
  LowStar_Vector_assign___uint8_t_(res, i1, h1);
  uint32_t j1 = i1 + (uint32_t)1U;
  if (j1 < res.sz)
  {
    return deserialize_hash_vec_i(hash_size, ok1, buf1, sz, pos1, res, j1);
  }
  return ((__bool_uint32_t){ .fst = true, .snd = pos1 });
}

static LowStar_Vector_vector_str___uint8_t_ alloc___uint8_t_(uint32_t len, uint8_t *v1)
{
  return LowStar_Vector_alloc_rid___uint8_t_(len, v1);
}

typedef struct __bool_uint32_t_LowStar_Vector_vector_str___uint8_t__s
{
  bool fst;
  uint32_t snd;
  LowStar_Vector_vector_str___uint8_t_ thd;
}
__bool_uint32_t_LowStar_Vector_vector_str___uint8_t_;

static __bool_uint32_t_LowStar_Vector_vector_str___uint8_t_
deserialize_hash_vec(
  uint32_t hash_size,
  bool ok,
  const uint8_t *buf1,
  uint32_t sz,
  uint32_t pos
)
{
  LowStar_Regional_regional__LowStar_Regional_regional__uint32_t__uint8_t__LowStar_Vector_vector_str___uint8_t_
  rg =
    {
      .state = {
        .state = hash_size,
        .dummy = hash_dummy,
        .r_alloc = hash_r_alloc,
        .r_free = hash_r_free
      },
      .dummy = hash_vec_dummy,
      .r_alloc = hash_vec_r_alloc,
      .r_free = hash_vec_r_free
    };
  if (!ok || pos >= sz)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str___uint8_t_){
          .fst = false,
          .snd = pos,
          .thd = LowStar_Regional_rg_dummy__LowStar_Vector_vector_str__uint8_t__LowStar_Regional_regional_uint32_t__uint8_t_(rg)
        }
      );
  }
  __bool_uint32_t_uint32_t scrut0 = deserialize_uint32_t(ok, buf1, sz, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  uint32_t n1 = scrut0.thd;
  if (!ok1)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str___uint8_t_){
          .fst = false,
          .snd = pos1,
          .thd = { .sz = (uint32_t)0U, .cap = (uint32_t)0U, .vs = NULL }
        }
      );
  }
  if (n1 == (uint32_t)0U)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str___uint8_t_){
          .fst = true,
          .snd = pos1,
          .thd = { .sz = (uint32_t)0U, .cap = (uint32_t)0U, .vs = NULL }
        }
      );
  }
  LowStar_Regional_regional__uint32_t__uint8_t_
  hrg =
    { .state = hash_size, .dummy = hash_dummy, .r_alloc = hash_r_alloc, .r_free = hash_r_free };
  LowStar_Vector_vector_str___uint8_t_
  res = alloc___uint8_t_(n1, LowStar_Regional_rg_dummy___uint8_t__uint32_t(hrg));
  __bool_uint32_t
  scrut = deserialize_hash_vec_i(hash_size, ok1, buf1, sz, pos1, res, (uint32_t)0U);
  bool ok2 = scrut.fst;
  uint32_t pos2 = scrut.snd;
  return
    ((__bool_uint32_t_LowStar_Vector_vector_str___uint8_t_){ .fst = ok2, .snd = pos2, .thd = res });
}

static __bool_uint32_t
deserialize_hash_vv_i(
  uint32_t hash_size,
  bool ok,
  const uint8_t *buf1,
  uint32_t sz,
  uint32_t pos,
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ res,
  uint32_t i1
)
{
  if (!ok || pos >= sz)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = (uint32_t)0U });
  }
  __bool_uint32_t_LowStar_Vector_vector_str___uint8_t_
  scrut = deserialize_hash_vec(hash_size, ok, buf1, sz, pos);
  bool ok1 = scrut.fst;
  uint32_t pos1 = scrut.snd;
  LowStar_Vector_vector_str___uint8_t_ hv = scrut.thd;
  if (!ok1)
  {
    return ((__bool_uint32_t){ .fst = false, .snd = pos1 });
  }
  LowStar_Vector_assign__LowStar_Vector_vector_str__uint8_t_(res, i1, hv);
  uint32_t j1 = i1 + (uint32_t)1U;
  if (j1 == res.sz)
  {
    return ((__bool_uint32_t){ .fst = true, .snd = pos1 });
  }
  return deserialize_hash_vv_i(hash_size, ok1, buf1, sz, pos1, res, j1);
}

static LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_
alloc__LowStar_Vector_vector_str__uint8_t_(
  uint32_t len,
  LowStar_Vector_vector_str___uint8_t_ v1
)
{
  return LowStar_Vector_alloc_rid__LowStar_Vector_vector_str__uint8_t_(len, v1);
}

typedef struct
__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t__s
{
  bool fst;
  uint32_t snd;
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ thd;
}
__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_;

static __bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_
deserialize_hash_vv(
  uint32_t hash_size,
  bool ok,
  const uint8_t *buf1,
  uint32_t sz,
  uint32_t pos
)
{
  if (!ok || pos >= sz)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_){
          .fst = false,
          .snd = pos,
          .thd = { .sz = (uint32_t)0U, .cap = (uint32_t)0U, .vs = NULL }
        }
      );
  }
  __bool_uint32_t_uint32_t scrut0 = deserialize_uint32_t(ok, buf1, sz, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  uint32_t n1 = scrut0.thd;
  if (!ok1)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_){
          .fst = false,
          .snd = pos1,
          .thd = { .sz = (uint32_t)0U, .cap = (uint32_t)0U, .vs = NULL }
        }
      );
  }
  if (n1 == (uint32_t)0U)
  {
    return
      (
        (__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_){
          .fst = true,
          .snd = pos1,
          .thd = { .sz = (uint32_t)0U, .cap = (uint32_t)0U, .vs = NULL }
        }
      );
  }
  LowStar_Regional_regional__LowStar_Regional_regional__uint32_t__uint8_t__LowStar_Vector_vector_str___uint8_t_
  rg =
    {
      .state = {
        .state = hash_size,
        .dummy = hash_dummy,
        .r_alloc = hash_r_alloc,
        .r_free = hash_r_free
      },
      .dummy = hash_vec_dummy,
      .r_alloc = hash_vec_r_alloc,
      .r_free = hash_vec_r_free
    };
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_
  res =
    alloc__LowStar_Vector_vector_str__uint8_t_(n1,
      LowStar_Regional_rg_dummy__LowStar_Vector_vector_str__uint8_t__LowStar_Regional_regional_uint32_t__uint8_t_(rg));
  __bool_uint32_t
  scrut = deserialize_hash_vv_i(hash_size, ok1, buf1, sz, pos1, res, (uint32_t)0U);
  bool ok2 = scrut.fst;
  uint32_t pos2 = scrut.snd;
  return
    (
      (__bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_){
        .fst = ok2,
        .snd = pos2,
        .thd = res
      }
    );
}

uint64_t mt_serialize_size(const merkle_tree *mt)
{
  merkle_tree mtv = *(merkle_tree *)mt;
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ hs = mtv.hs;
  LowStar_Vector_vector_str___uint8_t_ rhs = mtv.rhs;
  uint64_t hs_sz = hash_vv_bytes_i(mtv.hash_size, hs, (uint32_t)0U);
  if (hs_sz < (uint64_t)4294967295U)
  {
    uint64_t vs_hs = (uint64_t)rhs.sz * (uint64_t)mtv.hash_size;
    uint64_t ite;
    if (uint64_max - vs_hs >= (uint64_t)4U)
    {
      ite = vs_hs + (uint64_t)4U;
    }
    else
    {
      ite = uint64_max;
    }
    return (uint64_t)21U + hs_sz + (uint64_t)1U + ite + (uint64_t)mtv.hash_size;
  }
  return uint64_max;
}

uint64_t mt_serialize(const merkle_tree *mt, uint8_t *output, uint64_t sz)
{
  merkle_tree *mt1 = (merkle_tree *)mt;
  uint32_t sz1 = (uint32_t)sz;
  merkle_tree mtv = *mt1;
  __bool_uint32_t scrut = serialize_uint8_t(true, (uint8_t)1U, output, sz1, (uint32_t)0U);
  bool ok = scrut.fst;
  uint32_t pos = scrut.snd;
  __bool_uint32_t scrut0 = serialize_uint32_t(ok, mtv.hash_size, output, sz1, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  __bool_uint32_t scrut1 = serialize_offset_t(ok1, mtv.offset, output, sz1, pos1);
  bool ok2 = scrut1.fst;
  uint32_t pos2 = scrut1.snd;
  __bool_uint32_t scrut2 = serialize_uint32_t(ok2, mtv.i, output, sz1, pos2);
  bool ok3 = scrut2.fst;
  uint32_t pos3 = scrut2.snd;
  __bool_uint32_t scrut3 = serialize_uint32_t(ok3, mtv.j, output, sz1, pos3);
  bool ok4 = scrut3.fst;
  uint32_t pos4 = scrut3.snd;
  __bool_uint32_t scrut4 = serialize_hash_vv(mtv.hash_size, ok4, mtv.hs, output, sz1, pos4);
  bool ok5 = scrut4.fst;
  uint32_t pos5 = scrut4.snd;
  __bool_uint32_t scrut5 = serialize_bool(ok5, mtv.rhs_ok, output, sz1, pos5);
  bool ok6 = scrut5.fst;
  uint32_t pos6 = scrut5.snd;
  __bool_uint32_t scrut6 = serialize_hash_vec(mtv.hash_size, ok6, mtv.rhs, output, sz1, pos6);
  bool ok7 = scrut6.fst;
  uint32_t pos7 = scrut6.snd;
  __bool_uint32_t scrut7 = serialize_hash(mtv.hash_size, ok7, mtv.mroot, output, sz1, pos7);
  bool ok8 = scrut7.fst;
  uint32_t pos8 = scrut7.snd;
  if (ok8)
  {
    return (uint64_t)pos8;
  }
  return (uint64_t)0U;
}

merkle_tree
*mt_deserialize(
  uint32_t hash_size,
  const uint8_t *input,
  uint64_t sz,
  void (*hash_fun)(uint8_t *x0, uint8_t *x1, uint8_t *x2)
)
{
  uint32_t sz1 = (uint32_t)sz;
  __bool_uint32_t_uint8_t scrut0 = deserialize_uint8_t(true, input, sz1, (uint32_t)0U);
  bool ok = scrut0.fst;
  uint32_t pos = scrut0.snd;
  uint8_t format_version = scrut0.thd;
  bool ok1 = ok && format_version == (uint8_t)1U;
  __bool_uint32_t_uint32_t scrut1 = deserialize_uint32_t(ok1, input, sz1, pos);
  bool ok2 = scrut1.fst;
  uint32_t pos1 = scrut1.snd;
  uint32_t hsz = scrut1.thd;
  bool ok3 = ok2 && hsz == hash_size;
  __bool_uint32_t_uint64_t scrut2 = deserialize_offset_t(ok3, input, sz1, pos1);
  bool ok4 = scrut2.fst;
  uint32_t pos2 = scrut2.snd;
  uint64_t offset1 = scrut2.thd;
  __bool_uint32_t_uint32_t scrut3 = deserialize_index_t(ok4, input, sz1, pos2);
  bool ok5 = scrut3.fst;
  uint32_t pos3 = scrut3.snd;
  uint32_t i1 = scrut3.thd;
  __bool_uint32_t_uint32_t scrut4 = deserialize_index_t(ok5, input, sz1, pos3);
  bool ok6 = scrut4.fst;
  uint32_t pos4 = scrut4.snd;
  uint32_t j1 = scrut4.thd;
  __bool_uint32_t_LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_
  scrut5 = deserialize_hash_vv(hash_size, ok6, input, sz1, pos4);
  bool ok7 = scrut5.fst;
  uint32_t pos5 = scrut5.snd;
  LowStar_Vector_vector_str__LowStar_Vector_vector_str___uint8_t_ hs = scrut5.thd;
  __bool_uint32_t_bool scrut6 = deserialize_bool(ok7, input, sz1, pos5);
  bool ok8 = scrut6.fst;
  uint32_t pos6 = scrut6.snd;
  bool rhs_ok = scrut6.thd;
  __bool_uint32_t_LowStar_Vector_vector_str___uint8_t_
  scrut7 = deserialize_hash_vec(hash_size, ok8, input, sz1, pos6);
  bool ok9 = scrut7.fst;
  uint32_t pos7 = scrut7.snd;
  LowStar_Vector_vector_str___uint8_t_ rhs = scrut7.thd;
  __bool_uint32_t__uint8_t_ scrut = deserialize_hash(hash_size, ok9, input, sz1, pos7);
  bool ok10 = scrut.fst;
  uint8_t *mroot = scrut.thd;
  if
  (
    !ok10
    || false
    ||
      !(j1
      >= i1
      && uint64_max - offset1 >= (uint64_t)j1
      && hs.sz == (uint32_t)32U
      && rhs.sz == (uint32_t)32U)
  )
  {
    return NULL;
  }
  KRML_CHECK_SIZE(sizeof (merkle_tree), (uint32_t)1U);
  merkle_tree *buf = KRML_HOST_MALLOC(sizeof (merkle_tree));
  buf[0U]
  =
    (
      (merkle_tree){
        .hash_size = hash_size,
        .offset = offset1,
        .i = i1,
        .j = j1,
        .hs = hs,
        .rhs_ok = rhs_ok,
        .rhs = rhs,
        .mroot = mroot,
        .hash_fun = hash_fun
      }
    );
  return buf;
}

uint64_t
mt_serialize_path(
  const LowStar_Vector_vector_str___uint8_t_ *p1,
  const merkle_tree *mt,
  uint8_t *output,
  uint64_t sz
)
{
  merkle_tree x0 = *(merkle_tree *)mt;
  uint32_t hsz1 = x0.hash_size;
  uint32_t sz1 = (uint32_t)sz;
  LowStar_Vector_vector_str___uint8_t_ *ncp = (LowStar_Vector_vector_str___uint8_t_ *)p1;
  __bool_uint32_t scrut = serialize_uint32_t(true, hsz1, output, sz1, (uint32_t)0U);
  bool ok = scrut.fst;
  uint32_t pos = scrut.snd;
  __bool_uint32_t scrut0 = serialize_hash_vec(hsz1, ok, *ncp, output, sz1, pos);
  bool ok1 = scrut0.fst;
  uint32_t pos1 = scrut0.snd;
  if (ok1)
  {
    return (uint64_t)pos1;
  }
  return (uint64_t)0U;
}

LowStar_Vector_vector_str___uint8_t_
*mt_deserialize_path(uint32_t hsz, const uint8_t *input, uint64_t sz)
{
  uint32_t sz1 = (uint32_t)sz;
  __bool_uint32_t_uint32_t scrut0 = deserialize_uint32_t(true, input, sz1, (uint32_t)0U);
  bool ok = scrut0.fst;
  uint32_t pos = scrut0.snd;
  uint32_t hash_size = scrut0.thd;
  __bool_uint32_t_LowStar_Vector_vector_str___uint8_t_
  scrut = deserialize_hash_vec(hsz, ok, input, sz1, pos);
  bool ok1 = scrut.fst;
  LowStar_Vector_vector_str___uint8_t_ hs = scrut.thd;
  if (!ok1 || hash_size != hsz)
  {
    return NULL;
  }
  KRML_CHECK_SIZE(sizeof (LowStar_Vector_vector_str___uint8_t_), (uint32_t)1U);
  LowStar_Vector_vector_str___uint8_t_
  *buf = KRML_HOST_MALLOC(sizeof (LowStar_Vector_vector_str___uint8_t_));
  buf[0U] = hs;
  return buf;
}

