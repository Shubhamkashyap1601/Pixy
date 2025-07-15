# Pixy Chess Engine

Pixy is a powerful chess engine implemented in C++17 that combines traditional chess programming techniques with modern evaluation methods. It features both a command-line interface and a graphical user interface for an enhanced playing experience, with a particularly strong focus on proper chess principles and adaptive play strength.

## Features

- 🎮 Two interface options:
  - Command-line interface with colored pieces and clear board display
  - Graphical user interface for a more interactive experience
- 🤖 Strong chess AI implementation including:
  - Alpha-beta pruning with iterative deepening
  - Sophisticated move ordering
  - Transposition tables for improved performance
  - Advanced evaluation function considering:
    - Material balance
    - Piece positioning
    - King safety
    - Pawn structure
    - Piece activity
- 📊 Adaptive Play Strength:
  - Dynamic search depth adjustment based on game phase
  - Faster play in opening (depth 3)
  - Standard play in middlegame (depth 4)
  - Deeper calculation in endgame (depth 5)
- 🎯 Opening Principles:
  - Encourages proper development
  - Center control bonuses
  - Penalties for poor opening moves
  - Special handling of knight development
- ⚡ Performance Optimizations:
  - Move ordering with MVV-LVA
  - Late Move Reduction
  - Transposition table with 64K entries

## Project Structure

```
pixy/
├── src/
│   ├── main.cpp          # CLI entry point
│   ├── main_ui.cpp       # GUI entry point
│   ├── ai/               # AI engine components
│   │   ├── bot.cpp       # Main engine logic
│   │   └── evaluate.cpp  # Position evaluation
│   ├── core/            # Core chess logic
│   │   ├── board.cpp    # Board representation
│   │   ├── pieces/      # Individual piece logic
│   │   └── ...
│   ├── ui/             # User interface components
│   └── web/            # Web-related components
├── assets/            # Game assets
└── images/           # Piece images for GUI
```

## Building the Project

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- Make build system

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/Shubhamkashyap1601/Pixy.git
cd Pixy
```

2. Build the project:
```bash
make all
```

This will create two executables:
- `pixy` - Command-line interface
- `pixy-ui` - Graphical user interface

## Usage

### Command Line Interface
```bash
./pixy
```

### Graphical Interface
```bash
./pixy-ui
```

## Playing Chess

### Starting the Game
```bash
./pixy
```

### Main Menu Options
1. **Play vs Computer**
   - Choose your color (White/Black)
   - Adaptive AI strength
   - Clear move validation
   - Game state tracking
2. **Two Player Game**
   - Local multiplayer
   - Full move validation
   - Clear board display
3. **Load Position** (Coming Soon)
   - FEN position loader
4. **Settings** (Coming Soon)
   - Customize game options

### Making Moves
- Use algebraic notation (e.g., "e2 e4")
- Type 'q' to quit at any time
- Invalid moves are automatically rejected

### Computer Play Features
- Adaptive thinking depth based on game phase:
  - Opening (3-ply): Quick development
  - Middlegame (4-ply): Balanced calculation
  - Endgame (5-ply): Precise play
- Smart opening principles:
  - Center control bonus
  - Development incentives
  - Anti-primitive move penalties
- Sophisticated evaluation:
  - Material counting
  - Positional understanding
  - King safety evaluation
  - Pawn structure analysis

## Technical Implementation

### AI Components

- **Search Algorithm**: Alpha-beta pruning with iterative deepening
- **Move Ordering**: Implements smart move ordering for better pruning
- **Evaluation**: Sophisticated evaluation function considering multiple factors
- **Transposition Table**: 64K entry hash table for position caching

### Core Components

- Efficient board representation
- Legal move generation
- Game state management
- Individual piece movement logic

### User Interface

- Clean and intuitive graphical interface
- Real-time move validation
- Visual piece movement
- Game state display

## License

This project is open-source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit pull requests.

## Author

- [Shubham Kashyap](https://github.com/Shubhamkashyap1601)
