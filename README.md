# Midnight Dash 🏃‍♀️

A fast-paced endless runner game built with C++ and Raylib, featuring multiple characters, online leaderboards, and dynamic gameplay mechanics.

![Midnight Dash Gameplay](sample.gif)

## 🎮 Game Features

- **5 Unique Characters**: Choose from different avatars with unique abilities
  - Random Girl #1, #2, #3 (Classy, Demure, Mindful)
  - Intern Witch (Terrible at driving - special flying mechanics)
  - Unidentified Blue Dog (Doesn't talk)
- **Dynamic Gameplay**: 
  - Jump over obstacles to avoid losing lives
  - Collect coins to increase your score
  - Game speed increases over time for added challenge
- **Life System**: Start with 3 lives, lose one when hitting obstacles
- **Online Leaderboards**: Compete with players worldwide via Firebase integration
- **Local Score Tracking**: Personal best scores saved locally
- **Smooth Animations**: 6-frame character animations and coin effects

## 🎯 How to Play

### Controls
- **Space**: Jump (for ground characters)
- **W/S**: Move up/down (for flying character - Intern Witch)
- **A/D**: Move left/right
- **Mouse**: Navigate menus

### Objective
- Avoid obstacles to preserve your lives
- Collect coins to increase your score
- Survive as long as possible as the game speed increases
- Compete for the highest score on the global leaderboard

## 🛠️ Building and Running

### Prerequisites
- C++ compiler (GCC, Clang, or MSVC)
- [Raylib](https://www.raylib.com/) graphics library
- Windows (tested on Windows 10)

### Compilation
```bash
# Compile the main game
g++ -o main.exe main.cpp -lraylib -lopengl32 -lgdi32 -lwinmm

# Compile score upload utility
g++ -o uploadScore.exe uploadScore.cpp -lcurl

# Compile score download utility  
g++ -o getScores.exe getScores.cpp -lcurl
```

### Running
```bash
./main.exe
```

## 📁 Project Structure

```
midnight-dash/
├── main.cpp              # Main game logic and rendering
├── uploadScore.cpp       # Firebase score upload utility
├── getScores.cpp         # Firebase score download utility
├── json.hpp              # JSON library for Firebase communication
├── assets/               # Game assets
│   ├── background.png    # Scrolling background
│   ├── menus/           # UI backgrounds
│   ├── players/         # Character sprites (5 characters × 6 frames)
│   ├── obstacles/       # Obstacle sprites
│   └── other/           # Coins, hearts, etc.
├── scores               # Local score storage
├── highScores           # Downloaded leaderboard data
└── sample.gif           # Gameplay demonstration
```

## 🎨 Game Mechanics

### Character Abilities
- **Ground Characters**: Standard jump mechanics with space bar
- **Intern Witch**: Unique flying ability using W/S keys with reduced gravity
- **All Characters**: Horizontal movement with A/D keys

### Scoring System
- **Coins**: +10 points each
- **Obstacles**: Reduce lives by 1
- **Speed**: Increases by 5 FPS every obstacle spawn
- **Lives**: Start with 3, game over when reaching 0

### Online Features
- Real-time leaderboard updates via Firebase
- Score upload/download in background threads
- Top 5 global scores displayed

## 🔧 Technical Details

- **Engine**: Raylib (C graphics library)
- **Language**: C++17
- **Platform**: Windows (with potential for cross-platform)
- **Networking**: libcurl for Firebase API calls
- **Graphics**: 1024×768 resolution, 120+ FPS
- **Threading**: Background score synchronization

## 🎮 Game States

1. **Menu**: Main menu with play, instructions, credits, exit
2. **Character Selection**: Choose your avatar
3. **Gameplay**: Main game loop
4. **Instructions**: Control guide
5. **Credits**: Game credits
6. **Game Over**: Score display and restart option


## 📝 License

This project is open source. Feel free to contribute, modify, and distribute.

---

**Enjoy running through the midnight!** 🌙✨

aaa
