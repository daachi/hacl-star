module Spec.AEAD

open FStar.Mul
open Lib.IntTypes
open Lib.Sequence
open Lib.ByteSequence


#set-options "--z3rlimit 25 --max_fuel 1"

type algorithm =
  | AEAD_AES128_GCM
  | AEAD_Chacha20_Poly1305

inline_for_extraction
let size_key (a:algorithm) : Tot size_nat =
  match a with
  | AEAD_AES128_GCM -> 16
  | AEAD_Chacha20_Poly1305 -> 32

val size_key_lemma: a: algorithm -> Lemma
  (size_key a >= 16 /\ size_key a <= 32)
  [SMTPat (size_key a)]
let size_key_lemma a = ()


inline_for_extraction
let size_block (a:algorithm) : Tot size_nat =
  match a with
  | AEAD_AES128_GCM -> 16 // Spec.AES128_GCM.size_block
  | AEAD_Chacha20_Poly1305 -> 16 // Spec.Chacha20Poly1305.size_block

inline_for_extraction
let size_tag (a:algorithm) : Tot size_nat =
  match a with
  | AEAD_AES128_GCM -> 16 // Spec.AES128_GCM.size_tag
  | AEAD_Chacha20_Poly1305 -> 16 // Spec.Chacha20Poly1305.size_tag

inline_for_extraction
let size_nonce (a:algorithm) : Tot size_nat =
  match a with
  | AEAD_AES128_GCM -> 12 // Spec.AES128_GCM.size_nonce
  | AEAD_Chacha20_Poly1305 -> 12 // Spec.Chacha20Poly1305.size_nonce

val size_nonce_lemma: a: algorithm -> Lemma
  (size_nonce a >= 12 /\ size_nonce a <= 12)
  [SMTPat (size_nonce a)]
let size_nonce_lemma a = ()

inline_for_extraction
let padlen (a:algorithm) (x:size_nat) : size_nat =
  ((size_block a - x % (size_block a)) % (size_block a))


/// Types

type key (a:algorithm) = lbytes (size_key a)
type block (a:algorithm) = lbytes (size_block a)
type nonce (a:algorithm) = lbytes (size_nonce a)
type tag (a:algorithm) = lbytes (size_tag a)


/// API

val aead_encrypt:
    a: algorithm
  -> k: key a
  -> n: nonce a
  -> m: bytes{length m <= max_size_t
           /\ length m + size_block a <= max_size_t
           /\ length m + padlen a (length m) <= max_size_t}
  -> aad: bytes {length aad <= max_size_t /\ length aad + padlen a (length aad) <= max_size_t} ->
  Tot (lbytes (length m + size_tag a))

let aead_encrypt a k n m aad =
  match a with
  | AEAD_AES128_GCM -> Spec.AES128_GCM.aead_encrypt k n m aad
  | AEAD_Chacha20_Poly1305 -> Spec.Chacha20Poly1305.aead_encrypt k n m aad


val aead_decrypt:
    a: algorithm
  -> k: key a
  -> n: nonce a
  -> c: bytes{length c + size_block a <= max_size_t}
  -> mac: tag a
  -> aad: bytes{length aad <= max_size_t
             /\ (length c + length aad) / 64 <= max_size_t
             /\ length aad + padlen a (length aad) <= max_size_t} ->
  Tot (option (lbytes (length c)))

let aead_decrypt a k n c mac aad =
  match a with
  | AEAD_AES128_GCM -> Spec.AES128_GCM.aead_decrypt k n c mac aad
  | AEAD_Chacha20_Poly1305 -> Spec.Chacha20Poly1305.aead_decrypt k n c mac aad
