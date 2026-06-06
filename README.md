# 俄罗斯方块：技能大作战

一个基于 `C++17 + Qt + CMake` 实现的俄罗斯方块项目。在传统玩法基础上，加入了技能系统、分数成长解锁、本地排行榜，以及挑战模式下的手动配装机制。

## 核心特性

- 经典模式：纯粹的俄罗斯方块玩法，无技能干扰
- 无限挑战模式：通过消行积累能量，在局内释放不同技能
- 本地排行榜：自动记录最高分和本地 Top 5 成绩
- 成长解锁：随着历史最高分提升，逐步解锁新技能与更多携带槽位
- 手动配装：挑战模式开局前可从已解锁技能中手动选择本局携带技能
- 公共能量池：所有技能共用同一个能量池，不同技能消耗不同

## 游戏模式

### 经典模式

- 不启用技能
- 不积累技能能量
- 分数仍会进入本地排行榜，并可推动技能解锁进度

### 无限挑战模式

- 启用技能和能量系统
- 进入对局前必须手动选满当前可用槽位数
- 局内 `1-4` 键的技能顺序与主菜单中的配装顺序一致

## 键位说明

| 按键 | 功能 |
| --- | --- |
| `← / A` | 向左移动 |
| `→ / D` | 向右移动 |
| `↑ / W` | 顺时针旋转 |
| `Z` | 逆时针旋转 |
| `↓ / S` | 软降 |
| `Space` | 硬降 |
| `1 / 2 / 3 / 4` | 释放对应槽位技能 |
| `V / E` | 释放第 1 个技能 |
| `Esc / P` | 暂停 |

## 排行榜与本地存档

项目会在本地保存以下进度：

- 历史最高分
- 本地 Top 5 排行榜
- 已解锁技能列表
- 当前可用技能槽位数
- 手动选择的挑战模式配装

说明：

- 这些数据保存在系统本地配置目录中，不在仓库内
- 删除仓库目录不会自动清空本地排行榜
- 这不是联网排行榜，不会同步到远程服务器

## 技能系统

### 能量规则

当前公共能量池上限为 `140`。

| 消行数 | 获得能量 |
| --- | --- |
| 1 行 | `+20` |
| 2 行 | `+35` |
| 3 行 | `+50` |
| 4 行 | `+80` |
| 连击奖励 | 每层额外 `+5` |

技能不再要求“能量满才可释放”，而是按各自的能量消耗判断是否可用。

### 解锁与槽位成长

默认从 2 个低消耗技能起步，随着历史最高分提升逐步解锁更多技能和槽位：

| 历史最高分 | 解锁内容 | 可携带槽位 |
| --- | --- | --- |
| `0` | `time_freeze`、`bottom_blast` | 2 |
| `1200` | `line_clear` | 2 |
| `3000` | `earthquake` | 3 |
| `5500` | `gravity_break` | 3 |
| `9000` | `purge_wave` | 4 |

### 手动配装规则

- 只对挑战模式生效
- 必须恰好选满当前可用槽位数
- 不能重复选择同一个技能
- 选择顺序决定局内 `1-4` 键顺序
- 选择结果会本地持久化，下次启动默认沿用
- 如果后续解锁了新技能或新槽位，系统会尽量保留旧配装，并自动补齐到合法状态

### 当前技能列表

| 类型 ID | 名称 | 效果 | 消耗 | 解锁分数 |
| --- | --- | --- | --- | --- |
| `time_freeze` | 时间凝滞 | 显著降低当前下落速度，适合紧急救场 | `35` | `0` |
| `bottom_blast` | 底部爆破 | 清除最底部 1 行，并让上方方块整体下落 | `45` | `0` |
| `line_clear` | 直线清扫 | 清除当前最危险的一行，适合快速拆弹 | `60` | `1200` |
| `earthquake` | 大地震 | 强制清除最底部 2 行，快速压低堆叠 | `80` | `3000` |
| `gravity_break` | 失重领域 | 短时间内极大降低下落速度，争取重整棋形 | `95` | `5500` |
| `purge_wave` | 净场脉冲 | 清除底部 4 行，代价极高，但能强行续命 | `120` | `9000` |

## 构建与运行

### 环境要求

- Qt `6.x`
- CMake `3.16+`
- 支持 `C++17` 的编译器
- Windows 下推荐 `MinGW + Ninja`

### 命令行构建（Windows / Qt MinGW）

```powershell
$env:PATH = 'C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;' + $env:PATH
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64
cmake --build build
.\build\TetrisSkillBattle.exe
```

### Qt Creator

1. 打开项目根目录下的 `CMakeLists.txt`
2. 选择可用的 Qt / MinGW Kit
3. 点击构建并运行

## 项目结构

```text
TetrisSkillBattle/
├─ CMakeLists.txt
├─ README.md
├─ src/
│  ├─ main.cpp
│  ├─ core/
│  │  ├─ GameConfig.h
│  │  ├─ GameEngine.h/.cpp
│  │  ├─ ProgressManager.h/.cpp
│  │  ├─ RandomBag.h/.cpp
│  │  ├─ ScoreCalculator.h/.cpp
│  │  ├─ SkillManager.h/.cpp
│  │  ├─ TetrisBoard.h/.cpp
│  │  ├─ Tetromino.h/.cpp
│  │  └─ skills/
│  │     ├─ SkillBase.h/.cpp
│  │     ├─ SkillFactory.h/.cpp
│  │     └─ 各技能实现
│  ├─ data/
│  │  └─ skills.json
│  └─ ui/
│     ├─ MainWindow.h/.cpp
│     ├─ MenuWidget.h/.cpp
│     ├─ SkillPanelWidget.h/.cpp
│     ├─ GameBoardWidget.h/.cpp
│     ├─ EnergyBarWidget.h/.cpp
│     ├─ ScorePanelWidget.h/.cpp
│     ├─ GameOverDialog.h/.cpp
│     └─ effects/
└─ python/
   ├─ skill_editor.py
   └─ generate_skills_json.py
```

## 扩展技能

### 代码层面

1. 在 `src/core/skills/` 下新增一个继承 `SkillBase` 的技能类
2. 实现 `onActivate(...)`，按需要实现 `onTick(...)` / `onDeactivate(...)`
3. 使用 `REGISTER_SKILL(YourSkill, "your_skill_id")` 注册
4. 在 `src/data/skills.json` 中补充对应元数据
5. 重新构建项目

示例：

```cpp
class NewSkill : public SkillBase {
public:
    void onActivate(TetrisBoard& board, GameEngine& engine) override;

    QString name() const override { return QStringLiteral("新技能"); }
    QString description() const override { return QStringLiteral("技能说明"); }
    QString typeId() const override { return QStringLiteral("new_skill"); }
};

REGISTER_SKILL(NewSkill, "new_skill")
```

### `skills.json` 的作用

`skills.json` 主要用于存放技能的展示性元数据，包括：

- 技能类型 ID
- 名称
- 描述
- 主题色
- 是否为瞬发技能
- 持续时间
- 能量消耗

当前运行时的核心逻辑仍以 C++ 实现为准，`skills.json` 主要用于数据整理和辅助工具。

## Python 工具

仓库包含一些辅助脚本，用于维护技能数据：

```powershell
python python/skill_editor.py
python python/generate_skills_json.py import skills.csv
python python/generate_skills_json.py export skills.csv
```

## 提交说明

仓库不会提交以下本地运行产物：

- `build/` 编译目录
- 可执行文件
- 日志、缓存、编辑器临时文件
- 本地排行榜和配装存档
