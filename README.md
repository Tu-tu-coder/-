# 俄罗斯方块：技能大作战 (Tetris: Skill Battle)

经典俄罗斯方块 + 技能系统，C++ / Qt 实现。

## 快速开始

### 环境要求
- Qt 6.x (或 Qt 5.15+)
- CMake 3.16+
- C++17 编译器 (MinGW / MSVC)

### 构建 (使用 Qt MinGW)

```bash
# 设置环境
export PATH="D:/Qt/tools/CMake_64/bin:D:/Qt/tools/mingw1310_64/bin:$PATH"

# 配置
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH=D:/Qt/6.11.1/mingw_64

# 编译
cmake --build build

# 运行
./build/TetrisSkillBattle.exe
```

### 使用 Qt Creator
1. 打开 `CMakeLists.txt`
2. 选择 MinGW 工具链
3. 点击构建 → 运行

## 操作说明

| 按键 | 功能 |
|------|------|
| ← → / A D | 左右移动 |
| ↑ / W | 顺时针旋转 |
| Z | 逆时针旋转 |
| ↓ / S | 软降 (加速下落) |
| Space | 硬降 (直接落底) |
| V / E | 释放技能 (能量满时) |
| Esc / P | 暂停 |

## 游戏模式

- **经典模式**：传统俄罗斯方块，无技能
- **无限挑战模式**：消行充能 → 释放技能

## 技能系统

| 技能 | 效果 | 消耗 |
|------|------|------|
| 时间凝滞 | 方块下落速度降低80%，持续6秒 | 100 能量 |
| 大地震 | 消除底部2行，上方方块下落 | 100 能量 |

## 能量规则

| 消行数 | 能量获得 |
|--------|---------|
| 1 行 | +20 |
| 2 行 | +35 |
| 3 行 | +50 |
| 4 行 | +80 |
| 连击 | 每次额外+5 |

能量上限 100，满后按 V 释放技能。

## 添加新技能

1. 创建 `src/core/skills/NewSkill.h` (继承 `SkillBase`)
2. 创建 `src/core/skills/NewSkill.cpp` (实现 + `REGISTER_SKILL` 宏)
3. 在 `src/data/skills.json` 中添加条目
4. 重新编译

示例：
```cpp
// NewSkill.h
class NewSkill : public SkillBase {
    void onActivate(TetrisBoard& board, GameEngine& engine) override;
    QString name() const override { return "新技能"; }
    QString typeId() const override { return "new_skill"; }
    // ...
};

// NewSkill.cpp
REGISTER_SKILL(NewSkill, "new_skill")
```

## 项目结构

```
TetrisSkillBattle/
├── CMakeLists.txt              # 根CMake配置
├── src/
│   ├── main.cpp                # 入口
│   ├── core/                   # 游戏核心逻辑
│   │   ├── GameConfig.h        # 常量配置
│   │   ├── Tetromino.h/cpp     # 7种方块 + SRS旋转
│   │   ├── TetrisBoard.h/cpp   # 棋盘/碰撞/消行
│   │   ├── RandomBag.h/cpp     # 7-bag随机
│   │   ├── ScoreCalculator.h/cpp
│   │   ├── GameEngine.h/cpp    # 主循环/状态机
│   │   ├── SkillManager.h/cpp  # 能量/技能管理
│   │   └── skills/             # 技能实现
│   ├── ui/                     # Qt界面层
│   │   ├── MainWindow.h/cpp
│   │   ├── GameBoardWidget.h/cpp
│   │   ├── EnergyBarWidget.h/cpp
│   │   └── effects/            # 视觉效果
│   └── data/
│       └── skills.json         # 技能数据
├── python/                     # Python辅助工具
│   ├── skill_editor.py         # 技能参数GUI编辑器
│   └── generate_skills_json.py # CSV↔JSON转换
└── README.md
```

## Python 工具

```bash
# 技能编辑器 (tkinter GUI)
python python/skill_editor.py

# CSV → JSON
python python/generate_skills_json.py import skills.csv

# JSON → CSV
python python/generate_skills_json.py export skills.csv
```
