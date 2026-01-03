# 🏙️ Dhaka City Simulation - OpenGL

A comprehensive animated city simulation showcasing Dhaka's urban landscape with dynamic weather, day/night cycles, and realistic traffic flow.

![OpenGL](https://img.shields.io/badge/OpenGL-GLUT-blue)
![C++](https://img.shields.io/badge/C++-11-green)
![License](https://img.shields.io/badge/license-MIT-orange)

---

## 📸 Preview

### 1. Day View
<img width="1920" height="1080" alt="Day View" src="https://github.com/user-attachments/assets/c222172b-b3de-483c-aa19-90f0fa1bf6c1" />

### 2. Night View
<img width="1920" height="1080" alt="Night View" src="https://github.com/user-attachments/assets/38fa41c6-6d35-4d1a-98c9-ff32f571e5dc" />

### 3. Day with Rain
<img width="1920" height="1080" alt="Day with Rain" src="https://github.com/user-attachments/assets/5d19fc8d-fc14-44f3-827f-a0328cb4b8dd" />

### 4. Night with Rain
<img width="1920" height="1080" alt="Night with Rain" src="https://github.com/user-attachments/assets/3816d0a6-9cf5-480c-9739-2be0f2c4d925" />

---

## ✨ Features Overview

A fully animated 2D city scene featuring:

- 🏗️ **13+ Detailed Buildings** (Hospitals, Schools, Police Stations, Shopping Malls, Hotels)
- 🚇 **Metro Rail System** with functional station stops
- 🚗 **Multi-lane Traffic** across 3 roads with cars, buses, and trucks
- 👥 **Walking Pedestrians** with umbrellas during rain
- 🌦️ **Dynamic Weather** (rain effects with sound)
- 🌓 **Day/Night Cycle** (automatic 20-second transitions)
- 💡 **Street Lighting** (glowing at night)
- ✈️ **Flying Airplane**
- 🎵 **Background Audio** (traffic and metro sounds)

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `D` or `d` | Switch to **Day Mode** |
| `N` or `n` | Switch to **Night Mode** |
| `R` or `r` | Toggle **Rain ON/OFF** |
| `ESC` | Exit Program |

---

## 🚀 Detailed Features

### 🌆 Dynamic Environment

- **Automatic Day/Night Cycle**: Smooth transitions every 20 seconds
- **Weather System**: Realistic rain animation with diagonal drops and sound effects
- **Lighting Effects**: Street lights glow at night, building windows illuminate with warm colors
- **Sky Dynamics**: Moving clouds, moon at night, airplane flying across the sky

### 🚦 Traffic System

- **3 Roads**: Each with bidirectional traffic flow
- **9+ Vehicles**: Cars, buses, and trucks with realistic headlights at night
- **Realistic Movement**: Different speeds and lane behaviors
- **Night Headlights**: Vehicles emit light beams in night mode

### 🚇 Metro Rail

- **Elevated Track Structure**: Inspired by Dhaka Metro MRT-6
- **Intelligent Station Stops**: Train decelerates, stops for 3 seconds, then continues
- **Bangladesh Colors**: Red and green accents representing the national flag
- **Detailed Design**: Pantograph, wheels, windows, and authentic color scheme

### 🏘️ Buildings

Includes 13+ diverse structures:

- 🏥 **Hospital** - White building with red cross (lit at night)
- 🏫 **School** - Yellow building with clock tower
- 🚔 **Police Station** - Blue building with badge emblem
- 🛍️ **Shopping Mall** - Modern glass design with large display windows
- 🏨 **Luxury Hotel** - Multi-story with golden dome restaurant
- 🌿 **Eco-Green Building** - Solar panels & vertical gardens
- 🏢 **Modern Glass Towers** - Reflective windows
- 📚 **Library** - Classic architecture with columns
- 🍽️ **Restaurant** - Striped awning and large windows
- 🏢 **Apartment Buildings**
- 🏗️ **Various Commercial Buildings**

### 👥 Pedestrians

- **Walking Animations**: Realistic leg movement
- **Smart Umbrellas**: Automatically appear during rain
- **Multiple Characters**: Different clothing colors and walking directions
- **Dual Directions**: People walking both left and right

---

## 🛠️ Installation

### Prerequisites

- **C++ Compiler** (GCC/MinGW recommended)
- **OpenGL/GLUT Library** (FreeGLUT)
- **Windows Multimedia Library** (for audio)
- **Code::Blocks IDE** (recommended) or any C++ IDE

### Setup Instructions

#### Step 1: Install Code::Blocks
Download and install Code::Blocks with MinGW from [codeblocks.org](http://www.codeblocks.org/downloads)

#### Step 2: Configure OpenGL Libraries

1. Open your project in Code::Blocks
2. Go to **Projects → Build Options → Linker Settings**
3. Add the following libraries:

<img width="1920" height="1080" alt="Linker Settings" src="https://github.com/user-attachments/assets/65f5a0fd-1f24-48ae-a598-39e4fb7b64c4" />
