module Hacl.Impl.SHA2

open FStar.Mul
open FStar.HyperStack
open FStar.HyperStack.ST
open Spec.Lib.IntTypes

open Spec.Lib.IntBuf
open Spec.Lib.IntBuf.Lemmas
open Spec.SHA2

module ST = FStar.HyperStack.ST
module LSeq = Spec.Lib.IntSeq
module Spec = Spec.SHA2


inline_for_extraction
let v = size_v
inline_for_extraction
let index (x:size_nat) = size x

(* Define algorithm parameters *)
inline_for_extraction let size_hash_w : size_t = size Spec.size_hash_w
inline_for_extraction let size_block_w : size_t = size Spec.size_block_w
inline_for_extraction let size_block (p:Spec.parameters) = size (Spec.size_block p)
inline_for_extraction let max_input (p:Spec.parameters) = Spec.max_input p


(* Definition: Types for block and hash as sequences of words *)
type block_w (p:Spec.parameters) = b:lbuffer (uint_t p.wt) 16
type hash_w  (p:Spec.parameters) = b:lbuffer (uint_t p.wt) Spec.size_hash_w

unfold type kTable (p:Spec.parameters) = b:lbuffer (uint_t p.wt) p.kSize
unfold type wsTable (p:Spec.parameters) = b:lbuffer (uint_t p.wt) p.kSize

(* Definition of the scheduling function (part 1) *)
let step_ws0 (p:Spec.parameters) (b:block_w p) (i:size_t{size_v i < 16}) (s:wsTable p)
: Stack unit
  (requires (fun h -> live h s /\ live h b))
  (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 s h0 h1
            /\ as_lseq s h1 == Spec.step_ws0 p (as_lseq b h0) (size_v i) (as_lseq s h0))) =
  s.(i) <- b.(i)

#set-options "--max_fuel 0 --z3rlimit 50"

(* Definition of the scheduling function (part 2) *)
let step_ws1 (p:Spec.parameters) (i:size_t{size_v i >= 16 /\ size_v i < p.kSize}) (s:wsTable p)
: Stack unit
  (requires (fun h -> live h s))
  (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 s h0 h1
                       /\ as_lseq s h1 == Spec.step_ws1 p (size_v i) (as_lseq s h0))) =
  let t16 = s.(i -. size 16) in
  let t15 = s.(i -. size 15) in
  let t7  = s.(i -. size 7) in
  let t2  = s.(i -. size 2) in
  let s1 = _sigma1 p t2 in
  let s0 = _sigma0 p t15 in
  s.(i) <- s1 +. (t7 +. (s0 +. t16))

(* Definition of the core scheduling function *)
let ws (p:Spec.parameters) (s:wsTable p) (b:block_w p) :
  Stack unit
        (requires (fun h -> live h s /\ live h b))
        (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 s h0 h1)) =
  (**) let h0 = ST.get () in
  //let lseq_b = as_lseq b h0 in
  let lseq_b = LSeq.create 16 (nat_to_uint #p.wt 0) in
  let spec0 (i:size_nat{i < 16}) (s:Spec.Lib.IntSeq.intseq p.wt p.kSize) : Tot (Spec.Lib.IntSeq.intseq p.wt p.kSize) = Spec.step_ws0 p lseq_b i s in
  let spec1 (i:size_nat{i + 16 < p.kSize}) (s:Spec.Lib.IntSeq.intseq p.wt p.kSize) : Tot (Spec.Lib.IntSeq.intseq p.wt p.kSize) = Spec.step_ws1 p (i + 16) s in
  iteri (size 16) spec0 (step_ws0 p b) s;
  iteri (size p.kSize -. size 16) spec1 (fun i -> step_ws1 p (i +. size 16)) s

(* Definition of the core shuffling function *)
let shuffle_core (p:Spec.parameters) (kt:kTable p) (wst:wsTable p) (t:size_nat{t < p.kSize}) (hash:hash_w p)
: Stack unit
        (requires (fun h -> live h kt /\ live h wst /\ live h hash))
        (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 hash h0 h1)) =
  let a0 = hash.(size 0) in
  let b0 = hash.(size 1) in
  let c0 = hash.(size 2) in
  let d0 = hash.(size 3) in
  let e0 = hash.(size 4) in
  let f0 = hash.(size 5) in
  let g0 = hash.(size 6) in
  let h0 = hash.(size 7) in

  let wst_t = wst.(size t) in
  let kt_t = kt.(size t) in
  let t1 = h0 +. (_Sigma1 p e0) +. ((_Ch p e0 f0 g0) +. (kt_t +. wst_t)) in
  let t2 = (_Sigma0 p a0) +. (_Maj p a0 b0 c0) in

  hash.(size 0) <- (t1 +. t2);
  hash.(size 1) <- a0;
  hash.(size 2) <- b0;
  hash.(size 3) <- c0;
  hash.(size 4) <- (d0 +. t1);
  hash.(size 5) <- e0;
  hash.(size 6) <- f0;
  hash.(size 7) <- g0

// Definition of the full shuffling function
assume val shuffle: (p:Spec.parameters) -> (kt:kTable p) -> (wst:wsTable p) -> (hash:hash_w p) ->
  Stack unit
        (requires (fun h -> live h kt /\ live h wst))
        (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 hash h0 h1))
// let shuffle (p:parameters) (kt:kTable p) (wst:wsTable p) (hash:hash_w p)
// : Stack unit
//         (requires (fun h -> live h kt /\ live h wst))
//         (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 hash h0 h1)) =
//   let lseq_wst = LSeq.create (p.kSize) (nat_to_uint #p.wt 0) in
//   iter (size p.kSize) (Spec.shuffle_core p lseq_wst) (shuffle_core p kt wst) hash

(* Definition of the SHA2 state *)
let len_block_t (p:Spec.parameters) = l:size_t{uint_v l < Spec.size_block p}

noeq type state (p:Spec.parameters) =
  {
    hash:lbuffer (uint_t p.wt) Spec.size_hash_w;
    k:lbuffer (uint_t p.wt) p.kSize;
    ws:lbuffer (uint_t p.wt) p.kSize;
    block:lbuffer (uint_t p.wt) Spec.size_block_w;
    len_block:len_block_t p;
    n:lbuffer (uint_t p.wt) 1;
    tmp_block:lbuffer (uint_t p.wt) Spec.size_block_w;
  }


let set_h0 (p:parameters) (hw:hash_w p) :
  Stack unit
        (requires (fun h -> live h hw))
        (ensures  (fun h0 _ h1 -> preserves_live h0 h1 /\ modifies1 hw h0 h1
                             /\ as_lseq hw h1 == p.h0)) =
  hw.(size 0) <- p.h0.[0];
  hw.(size 1) <- p.h0.[1];
  hw.(size 2) <- p.h0.[2];
  hw.(size 3) <- p.h0.[3];
  hw.(size 4) <- p.h0.[4];
  hw.(size 5) <- p.h0.[5];
  hw.(size 6) <- p.h0.[6];
  hw.(size 7) <- p.h0.[7];
  admit()

(* Definition of the initialization function for convenience *)
assume val init: p:parameters -> StackInline (state p)
                                            (requires (fun h -> True))
                                            (ensures  (fun h0 _ h1 -> True))

// let init (p:parameters) :
//   StackInline (state p)
//               (requires (fun h -> True))
//               (ensures  (fun h0 _ h1 -> True)) =
//   let h0 : hash_w p = create #(uint_t p.wt) size_hash_w (nat_to_uint 0) in
//   let ws0 : wsTable p = create #(uint_t p.wt) (uint_t p.kSize) (nat_to_uint 0) in
//   let block0 : block_w p = create #(uint_t p.wt) size_block_w (nat_to_uint 0) in
//   let tmp_block0 : block_w p = create #(uint_t p.wt) size_block_w (nat_to_uint 0) in
//   set_h0 p h0;
//   {hash = h0; ws = ws0; block = block0; len_block = size 0; n = size 0; tmp_block = tmp_block0}

(* Definition of the core compression function *)
let update_block (p:parameters) (block:lbuffer uint8 (Spec.size_block p)) (st:state p) =
  Spec.Lib.IntBuf.LoadStore.uints_from_bytes_be st.tmp_block block;
  ws p st.ws st.tmp_block;
  let hash1 = st.hash in
  shuffle p st.k st.ws st.hash;
  map2 (size p.size_hash) (fun x y -> x +. y) st.hash hash1;
  let n0 = st.n.(size 0) in
  st.n.(size 0) <- n0 +. (size 1)

(* Definition of the compression function iterated over multiple blocks *)
let update_multi (p:parameters) (n:size_t{uint_v n * Spec.size_block p <= max_size_t}) (blocks:lbytes (uint_v n * Spec.size_block p)) (st:state p{(as_lseq st.n h1).[0] + uint_v n <= max_size_t}) : Tot (st1:state p) =
  let bl = size_block p in
  let old_n = st.n in
  let old_len_block = st.len_block in
  iteri n (fun i -> update_block p (sub blocks (bl * i) bl)) st

(* Definition of the function for the partial block compression *)
let update_last (p:parameters) (len:size_nat) (last:lbytes len) (st:state p{len < size_block p /\ (st.n * size_block p) + len <= max_size_t})
: Tot (state p) =
  let blocks = pad_single p st.n len last in
  update_multi p (number_blocks_padding_single p len) blocks st

(* Definition of the finalization function *)
let finish p (hash:hash_w p) : lbytes p.size_hash =
  let hash_final = uints_to_bytes_be hash in
  let h = slice hash_final 0 p.size_hash in
  h
