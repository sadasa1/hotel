# Hotel — SFML Identity-Check Mini‑Game (C++)

> Papers-Please‑style desk loop: verify badges, flag anomalies, and manage a tiny hotel. Built in modern C++ with SFML and CMake.

![Demo gameplay](demo.gif)

## [[Full Demo](https://www.youtube.com/watch?v=Y1LotFzByAA)]

---

## Quickstart

> Works with SFML **2.6+ or 3.x**. Uses CMake + a single executable target `hotel`.

### macOS (Homebrew)
```bash
brew install cmake sfml
git clone https://github.com/sadasa1/hotel.git
cd hotel
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/hotel
```

### Windows (MSVC + vcpkg)
```powershell
git clone https://github.com/sadasa1/hotel.git
cd hotel

# One-time vcpkg bootstrap (installs in a local folder)
git clone https://github.com/microsoft/vcpkg.git
.cpkgootstrap-vcpkg.bat

# Install SFML (choose x64-windows)
.cpkgcpkg.exe install sfml:x64-windows

# Configure CMake to use vcpkg
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=.cpkg\scriptsuildsystems\ vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
.uild\Release\hotel.exe
```

> If you see linking errors, confirm SFML version matches your local headers (2.6 vs 3.x) and delete/rebuild the `build/` folder.

---

## What to look at (code tour)

- **`src/App.cpp`** — window + main loop (poll events → update → draw) + scene bootstrap
- **`src/SceneCheckIn.cpp`** — desk UI: Approve/Flag buttons, hover/press SFX, live portrait & badge rendering
- **`src/Hotel.cpp`** — game logic: round selection, badge verification, room assignment, anomaly injection
- **`src/Room.cpp`, `src/main.cpp`** — room slot logic & tiny launcher
- **`include/`** — public headers (`Hotel.h`, `SceneCheckIn.h`, `Room.h`, `Badge.h`, `Guest.h`)

Project layout:
```
assets/              # textures, audio, etc.
data/                # guests.tsv (tab-separated: DOB month/day/year, name, guestID, portraitID, ...)
include/             # headers
src/                 # C++ sources (App.cpp, Hotel.cpp, SceneCheckIn.cpp, Room.cpp, main.cpp)
docs/architecture.md # Mermaid diagrams of main loop & data flow
CMakeLists.txt
.clang-tidy          # (add .clang-format too)
```
> **`data/guests.tsv` schema (excerpt):** `MM<TAB>DD<TAB>YYYY<TAB>Name<TAB>GuestID<TAB>PortraitID`

---

## Architecture

See **[`architecture.md`](architecture.md)** for diagrams (main loop, scene/state, data flow).

High-level responsibilities:
- **App**: boots window, loads `data/guests.tsv`, selects round, pushes `SceneCheckIn`
- **SceneCheckIn**: purely presentation + input; calls `Hotel::loadClaimer()` then renders the badge/live
- **Hotel**: owns guests/rooms/inside set; validates IDs and injects anomalies; assigns/vacates rooms

---

## Game rules & anomalies (desk-only loop)

| Case | How it’s injected | What verification checks | Expected decision |
|---|---|---|---|
| **Clean** (~60%) | No change | ID format OK, portrait matches, not already inside | **Approve** |
| **Borrowed keycard** (~12%) | Overwrite badge ID with a guest ID that’s already inside; 5% of these also sync name/portrait | Unknown/duplicate-in, or conflict with inside-set | **Flag** |
| **Bad format / length** (~14%) | Append/remove a random char/digit, or delete first char | `isValidID` fails (non‑8‑digit / contains letters) | **Flag** |
| **Portrait mismatch** (~remainder) | Keep badge portrait; swap live portrait to a third person | `verifyBadge` detects mismatch | **Flag** |

_Next up_: connect Approve/Flag to logging & scoring; end-of-round summary; drag-to-inspect badge; “enter/exit” events that mutate room occupancy in real time.

---

## Building blocks (C++)

- C++20, STL (`<vector>`, `<unordered_map>`, `<optional>`, `<random>`)
- SFML Graphics/Audio (`sf::RenderWindow`, `sf::Sprite`, `sf::Sound`,`sf::Audio`)
- Clean separation: **App** (game loop), **Scene** (UI), **Hotel** (logic)

---
