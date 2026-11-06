#include <cstdint>
#include <effects/damage_pipeline.h>

#include <entities/elf-pet.h>
#include <fsm/battleContext.h>

void DamagePipeline::run(BattleContext* ctx, int attacker, int defender, uint32_t phase_mask) {
    if (!ctx) {
        return;
    }
    for (DamagePhase phase : kOrder) {
        if ((phase_mask & phase_bit(phase)) == 0) {
            continue;   // 该阶段被排除（属性伤害只跑子集，见头文件说明）
        }
        // 697/750/790「无视伤害限制」：本段 CAP 锁伤整体跳过（凭证在攻击后统一消费）。
        const bool cap_phase = phase == DamagePhase::CAP;
        const int cap_before = ctx->ws.resolvedDamage.final;
        if (cap_phase && attacker >= 0 && attacker <= 1
            && ctx->ws.attack_credential[attacker].ignore_damage_limit) {
            continue;
        }
        // 攻击方先结算，防御方后结算（顺序可随游戏知识调整）
        const int owners[2] = {attacker, defender};
        for (int owner : owners) {
            ElfPet& pet = ctx->getPet(owner);
            auto& list = buckets_[static_cast<int>(phase)][owner];
            for (auto& effect : list) {
                // 切换作废的条目跳过（惰性；真正的移除在下次 register_effect 压实）。
                if (!effect.team && effect.valid_id != ctx->pipeline_valid_id[owner]) {
                    continue;
                }
                const int cat_bit = 1 << static_cast<int>(effect.category);
                if (pet.damage_suppress_mask & cat_bit) {
                    continue;  // 该类别被抑制（如沧岚"挡伤失效"跳过对方的挡伤/检测）
                }
                if (effect.fn) {
                    effect.fn(ctx, owner);
                }
            }
        }
        if (cap_phase && ctx->ws.resolvedDamage.final < cap_before) {
            ctx->ws.resolvedDamage.capped = true;
        }
    }
}
