# ♟️ Chess Engine

A simple, performant chess engine written in C using bitboards and (optionally) magic bitboards for fast move generation.

---

## 🔧 Features
- Bitboard-based board representation
- FEN parsing and initialization
- Move generation (WIP)
- King safety and check detection
- Magic bitboards (planned or in-progress)

---

## 🚀 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/your-username/chess-engine.git
cd chess-engine
````

### 2. Build with CMake (Recommended)

#### Windows (MSVC):

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Linux/macOS (GCC/Clang):

```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run

```bash
./build/chess         # Linux/macOS
.\build\Debug\chess.exe   # Windows
```

---

## 🧠 Development Notes

* Magic bitboards will be used for rooks, bishops, and queens to speed up sliding piece attacks.
* Pawn and king attack masks are generated on the fly.
* Move generation and evaluation logic is modularized for future search integration (e.g. Minimax, MCTS, NNUE, etc).

---

## 📁 Project Structure

```
.
├── bitboard.c / .h       # Bitboard manipulation and attack masks
├── utils.c / .h          # Utility functions (e.g., popcount, random magic gen)
├── constants.h           # Piece indices, board constants
├── main.c                # Entry point
├── CMakeLists.txt        # CMake build script
└── README.md             # This file
```

---

## 📌 Todo

* [x] Bitboard setup from FEN
* [ ] Legal move generation
* [ ] Search and evaluation
* [ ] Magic bitboard precomputation
* [ ] GUI frontend (optional)

---

## 📜 License

MIT License. See `LICENSE` file for details.


