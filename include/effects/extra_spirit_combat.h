#ifndef EXTRA_SPIRIT_COMBAT_H
#define EXTRA_SPIRIT_COMBAT_H

// 可上场额外精灵的「真实参战数据」（2026-11-06，2584 瀚宇星皇）。
//
// 为什么单独成一个头、且由 ExtraSpirit 用 shared_ptr 持有：
//   spirit_lifecycle.h 被 battle_primitives.h → effect_unit.h → skills.h 这条链
//   反向包含；若 spirit_lifecycle.h 直接 include entities/skills.h，就会在
//   "effect_unit.h 先被包含"的顺序下形成 include 环（skills.h 拿到未定义的
//   EffectUnit）。把需要 Skills 的完整定义下沉到本头，只让真正消费参战数据的
//   .cpp / battleContext.h 包含它。
//
// 官方术语 70 说传统额外精灵"没有血量/PP/技能"；2584 是加入出战背包的特例，
// 它必须能被上场、能选招。传统额外精灵的 ExtraSpirit::combat 保持 nullptr。

#include <array>
#include <string>
#include <vector>

#include <entities/skills.h>

struct ExtraSpiritCombatData {
    int pet_id = 0;                // 物种 id
    std::string name;              // 参战名（"瀚宇星皇"）
    int element = 0;               // 主系别（与 ExtraSpirit::element 同步）
    int element2 = 0;              // 副系别
    int hp = 0;                    // 独立当前体力（未上场时的池值）
    int max_hp = 0;                // 独立体力上限
    std::array<int, 6> stats{};    // 战斗面板（NumericalPropertyIndex 序）
    std::vector<Skills> skills;    // 5 个可上场技能槽
    bool in_bag = false;           // 已实体化进 6 槽出战背包
    int bag_slot = -1;             // 实体化后的槽位（-1 = 尚未实体化）
    bool deployable = false;       // 是否可作为「额外上场槽」补位
};

#endif // EXTRA_SPIRIT_COMBAT_H
