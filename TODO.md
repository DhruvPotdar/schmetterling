# TODOS

- [ ] Precompute tables: knight/king, pawn attacks, rays, between masks
- [ ] Implement magic bitboards (or PEXT) for bishop/rook attacks
- [ ] Define AttackData: opp attack map, pin rays, check rays, king square
- [ ] Implement get_attackers_to and calculate_attack_data equivalents
- [ ] Legal king moves with opp attack map and castling safety checks
- [ ] Legal non-king moves with pin-ray and check-ray filtering
- [ ] Robust en passant legality via post-EP occupancy slider checks
- [ ] Forcing move gen: captures, promotions, quiet checks detection
- [ ] Pseudo-legal gen built on fast tables; keep for perft/divide
- [ ] Perft suite and known positions; compare against Stockfish/bench
- [ ] Hot path micro-opts: inlining, branchless masks, packed MoveBuffer
- [ ] Benchmark harness and nps reporting; profile cache misses
