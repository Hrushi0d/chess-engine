
# ♟️ Chess Engine Development – TODO List

## ✅ Completed

* [x] Bitboard-based board representation
* [x] Custom magic number generator for rooks (and soon bishops)
* [x] Move generation using magic bitboards
* [x] Move struct and full board state encapsulation
* [x] Make/Undo move system
* [x] Castling, en passant, and promotion handling
* [x] Perft testing for move validation
* [x] Initial evaluation function using material scores
* [x] TreeNode and MoveList structure
* [x] Magic constants output for integration with code

---

## 🛠 In Progress

* [ ] Generate Piece-Square Tables (PSTs) from game data
* [ ] Implement Zobrist hashing for transposition tables and repetition detection
* [ ] Add fast board hashing to support repetition rules and cacheable lookups
* [ ] Implement full move generation for bishop/queen using magic numbers
* [ ] Efficient move lookup table generation and loading from disk

---

## 🔜 Planned / Advanced Features

### Search Enhancements

* [ ] **Quiescence Search** — Avoid the horizon effect by exploring "quiet" positions after tactical moves
* [ ] **Static Exchange Evaluation (SEE)** — Estimate the outcome of capture sequences to avoid shallow blunders
* [ ] **Incremental Evaluation** — Update evaluation incrementally instead of recalculating from scratch
* [ ] **Principal Variation Search (PVS)** — Optimized form of alpha-beta pruning by assuming the best move first
* [ ] **Null Move Pruning** — Skip branches by assuming that a pass move is still better in quiet positions

### Evaluation and Strategy

* [ ] Dynamic evaluation using PSTs
* [ ] Phase-based evaluation (opening/midgame/endgame)
* [ ] King safety evaluation
* [ ] Mobility / space control heuristics

### Search Algorithm Alternatives

* [ ] Monte Carlo Tree Search (MCTS) as a pluggable alternative
* [ ] Iterative Deepening and aspiration windows
* [ ] Time management and move ordering

### Infrastructure

* [ ] Engine <-> GUI protocol (e.g., UCI support)
* [ ] PGN parser for training PSTs
* [ ] Save/load book openings and evaluation data
* [ ] ELO benchmarking against other engines or known perft counts

