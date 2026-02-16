-- ============================================
-- 植物戰紀 - 敵人定義
-- ============================================

local enemies = {}

--------------------------------------------------------------------------------
-- 基礎敵人 (★)
--------------------------------------------------------------------------------

enemies.corrupted_slime = {
    id = "corrupted_slime",
    name = "腐化史萊姆",
    name_en = "Corrupted Slime",
    description = "最基礎的敵人",
    tier = 1,
    
    stats = {
        hp = 50,
        speed = 30,
        damage = 20,
        armor = 0,
    },
    
    behavior = {
        type = "walker",  -- 直線前進
        attack_range = 30,
        attack_speed = 1.0,
    },
    
    drops = {
        exp = 5,
        coins = { min = 1, max = 2 },
    },
    
    animations = {
        idle = { fps = 4, frames = 4 },
        walk = { fps = 6, frames = 6 },
        attack = { fps = 8, frames = 4 },
        death = { fps = 10, frames = 6 },
    },
}

enemies.skeleton_minion = {
    id = "skeleton_minion",
    name = "骷髏小兵",
    name_en = "Skeleton Minion",
    description = "基礎骷髏士兵",
    tier = 1,
    
    stats = {
        hp = 80,
        speed = 40,
        damage = 25,
        armor = 0,
    },
    
    behavior = {
        type = "walker",
        attack_range = 40,
        attack_speed = 1.0,
    },
    
    drops = {
        exp = 8,
        coins = { min = 1, max = 3 },
    },
}

enemies.shadow_ghost = {
    id = "shadow_ghost",
    name = "暗影幽靈",
    name_en = "Shadow Ghost",
    description = "可以穿過植物",
    tier = 1,
    
    stats = {
        hp = 60,
        speed = 50,
        damage = 15,
        armor = 0,
    },
    
    behavior = {
        type = "phasing",  -- 可以穿過植物
        attack_range = 30,
        attack_speed = 1.5,
    },
    
    special = {
        can_phase = true,  -- 穿透
    },
    
    drops = {
        exp = 10,
        coins = { min = 2, max = 4 },
    },
}

enemies.corrupted_bat = {
    id = "corrupted_bat",
    name = "腐化蝙蝠",
    name_en = "Corrupted Bat",
    description = "飛行單位，速度極快",
    tier = 1,
    
    stats = {
        hp = 40,
        speed = 80,
        damage = 15,
        armor = 0,
    },
    
    behavior = {
        type = "flyer",  -- 飛行
        attack_range = 30,
        attack_speed = 2.0,
    },
    
    special = {
        is_flying = true,  -- 只能被特定植物攻擊
    },
    
    drops = {
        exp = 12,
        coins = { min = 2, max = 5 },
    },
}

enemies.corrupted_larva = {
    id = "corrupted_larva",
    name = "腐化幼蟲",
    name_en = "Corrupted Larva",
    description = "死亡後分裂成兩隻小蟲",
    tier = 1,
    
    stats = {
        hp = 20,
        speed = 25,
        damage = 10,
        armor = 0,
    },
    
    behavior = {
        type = "walker",
        attack_range = 20,
        attack_speed = 1.0,
    },
    
    special = {
        on_death = function(self)
            -- 分裂成兩隻更小的蟲
            for i = 1, 2 do
                game:spawn_enemy("corrupted_larva_mini", {
                    x = self.x + (i - 1.5) * 20,
                    y = self.y,
                })
            end
        end,
    },
    
    drops = {
        exp = 3,
        coins = { min = 0, max = 1 },
    },
}

--------------------------------------------------------------------------------
-- 精英敵人 (★★)
--------------------------------------------------------------------------------

enemies.bucket_skeleton = {
    id = "bucket_skeleton",
    name = "鐵桶骷髏",
    name_en = "Bucket Skeleton",
    description = "戴著鐵桶的高防禦骷髏",
    tier = 2,
    
    stats = {
        hp = 300,
        speed = 35,
        damage = 25,
        armor = 30,  -- 減傷 30%
    },
    
    behavior = {
        type = "walker",
        attack_range = 40,
        attack_speed = 1.0,
    },
    
    special = {
        armor_break_threshold = 100,  -- 受傷超過這個值，鐵桶掉落
        on_armor_break = function(self)
            self.stats.armor = 0
            self.stats.speed = self.stats.speed * 1.5
        end,
    },
    
    drops = {
        exp = 25,
        coins = { min = 5, max = 10 },
    },
}

enemies.berserk_werewolf = {
    id = "berserk_werewolf",
    name = "狂暴狼人",
    name_en = "Berserk Werewolf",
    description = "受傷後加速",
    tier = 2,
    
    stats = {
        hp = 200,
        speed = 50,
        damage = 35,
        armor = 0,
    },
    
    behavior = {
        type = "charger",  -- 衝鋒類型
        attack_range = 40,
        attack_speed = 1.5,
    },
    
    special = {
        enrage_threshold = 0.5,  -- HP < 50% 時狂暴
        on_enrage = function(self)
            self.stats.speed = self.stats.speed * 2.0
            self.stats.damage = self.stats.damage * 1.5
        end,
    },
    
    drops = {
        exp = 30,
        coins = { min = 6, max = 12 },
    },
}

enemies.voodoo_mage = {
    id = "voodoo_mage",
    name = "巫毒法師",
    name_en = "Voodoo Mage",
    description = "定期召喚小怪",
    tier = 2,
    
    stats = {
        hp = 150,
        speed = 30,
        damage = 20,
        armor = 0,
    },
    
    behavior = {
        type = "summoner",
        attack_range = 200,  -- 遠程
        attack_speed = 0.5,
        summon_interval = 8.0,
        summon_type = "corrupted_slime",
        summon_count = 3,
    },
    
    drops = {
        exp = 40,
        coins = { min = 8, max = 15 },
    },
}

enemies.dark_knight = {
    id = "dark_knight",
    name = "黑暗騎士",
    name_en = "Dark Knight",
    description = "快速衝鋒，撞擊植物",
    tier = 2,
    
    stats = {
        hp = 250,
        speed = 60,
        damage = 50,
        armor = 10,
    },
    
    behavior = {
        type = "charger",
        charge_speed = 150,
        charge_range = 200,
        attack_range = 50,
        attack_speed = 0.8,
    },
    
    special = {
        on_charge_hit = function(self, plant)
            -- 衝鋒命中造成額外傷害並擊退
            plant:take_damage(self.stats.damage * 2)
            plant:knockback(50)
        end,
    },
    
    drops = {
        exp = 35,
        coins = { min = 7, max = 14 },
    },
}

enemies.skeleton_archer = {
    id = "skeleton_archer",
    name = "骷髏弓手",
    name_en = "Skeleton Archer",
    description = "遠程攻擊",
    tier = 2,
    
    stats = {
        hp = 120,
        speed = 35,
        damage = 30,
        armor = 0,
    },
    
    behavior = {
        type = "ranged",
        attack_range = 300,
        attack_speed = 1.2,
    },
    
    projectile = {
        type = "arrow",
        speed = 400,
    },
    
    drops = {
        exp = 28,
        coins = { min = 5, max = 10 },
    },
}

enemies.shadow_assassin = {
    id = "shadow_assassin",
    name = "暗影刺客",
    name_en = "Shadow Assassin",
    description = "隱形接近，突然襲擊",
    tier = 2,
    
    stats = {
        hp = 180,
        speed = 70,
        damage = 60,
        armor = 0,
    },
    
    behavior = {
        type = "stealth",
        attack_range = 40,
        attack_speed = 2.0,
        reveal_range = 100,  -- 接近植物時現形
    },
    
    special = {
        invisible = true,
        backstab_mult = 2.0,  -- 背刺倍率
    },
    
    drops = {
        exp = 45,
        coins = { min = 10, max = 18 },
    },
}

--------------------------------------------------------------------------------
-- 高階敵人 (★★★)
--------------------------------------------------------------------------------

enemies.giant_zombie = {
    id = "giant_zombie",
    name = "巨型殭屍",
    name_en = "Giant Zombie",
    description = "超大血量，會投擲小怪",
    tier = 3,
    
    stats = {
        hp = 1200,
        speed = 25,
        damage = 80,
        armor = 20,
    },
    
    behavior = {
        type = "boss_lite",
        attack_range = 60,
        attack_speed = 0.5,
        throw_interval = 10.0,
        throw_type = "corrupted_slime",
    },
    
    special = {
        can_throw = true,
        throw_damage = 30,
    },
    
    drops = {
        exp = 150,
        coins = { min = 30, max = 50 },
    },
}

enemies.demon_guard = {
    id = "demon_guard",
    name = "惡魔守衛",
    name_en = "Demon Guard",
    description = "範圍攻擊，燃燒效果",
    tier = 3,
    
    stats = {
        hp = 1000,
        speed = 35,
        damage = 60,
        armor = 15,
    },
    
    behavior = {
        type = "area_attacker",
        attack_range = 80,
        attack_speed = 0.8,
    },
    
    special = {
        attack_type = "cleave",
        cleave_range = 100,
        burn_damage = 10,
        burn_duration = 3.0,
    },
    
    drops = {
        exp = 120,
        coins = { min = 25, max = 40 },
    },
}

enemies.baby_dragon = {
    id = "baby_dragon",
    name = "幼龍",
    name_en = "Baby Dragon",
    description = "噴射火焰，範圍燃燒",
    tier = 3,
    
    stats = {
        hp = 600,
        speed = 40,
        damage = 50,
        armor = 10,
    },
    
    behavior = {
        type = "breather",
        attack_range = 150,
        attack_speed = 0.6,
    },
    
    special = {
        breath_type = "fire",
        breath_angle = 45,
        breath_range = 150,
        burn_damage = 0.1,  -- 10% 攻擊力每秒
        burn_duration = 3.0,
    },
    
    drops = {
        exp = 100,
        coins = { min = 20, max = 35 },
    },
}

enemies.vampire_lord = {
    id = "vampire_lord",
    name = "吸血鬼領主",
    name_en = "Vampire Lord",
    description = "攻擊回復生命",
    tier = 3,
    
    stats = {
        hp = 900,
        speed = 55,
        damage = 45,
        armor = 0,
    },
    
    behavior = {
        type = "lifesteal",
        attack_range = 50,
        attack_speed = 1.2,
    },
    
    special = {
        lifesteal = 0.3,  -- 吸取 30% 傷害
        bat_spawn = {
            interval = 15.0,
            count = 3,
            type = "corrupted_bat",
        },
    },
    
    drops = {
        exp = 130,
        coins = { min = 28, max = 45 },
    },
}

--------------------------------------------------------------------------------
-- Boss (★★★★★)
--------------------------------------------------------------------------------

enemies.skeleton_king = {
    id = "skeleton_king",
    name = "骷髏王",
    name_en = "Skeleton King",
    description = "第一個 Boss",
    tier = "boss",
    
    stats = {
        hp = 5000,
        speed = 30,
        damage = 100,
        armor = 20,
    },
    
    phases = {
        {
            hp_percent = 1.0,
            behavior = {
                type = "summoner",
                attack_range = 80,
                attack_speed = 0.5,
                summon_interval = 10.0,
                summon_types = {"skeleton_minion", "bucket_skeleton"},
                summon_count = 4,
            },
        },
        {
            hp_percent = 0.5,
            behavior = {
                type = "berserk",
                attack_range = 100,
                attack_speed = 1.0,
                damage_mult = 1.5,
                speed_mult = 1.5,
            },
            on_enter = function(self)
                -- 進入第二階段
                game:show_text("骷髏王進入狂暴狀態！")
            end,
        },
    },
    
    drops = {
        exp = 1000,
        coins = { min = 200, max = 300 },
        guaranteed = {"rare_plant_card"},
    },
    
    animations = {
        idle = { fps = 4, frames = 8 },
        walk = { fps = 6, frames = 8 },
        attack = { fps = 8, frames = 10 },
        summon = { fps = 6, frames = 12 },
        rage = { fps = 10, frames = 6 },
        death = { fps = 8, frames = 16 },
    },
}

enemies.bone_dragon = {
    id = "bone_dragon",
    name = "骨龍",
    name_en = "Bone Dragon",
    description = "巨型飛行 Boss",
    tier = "boss",
    
    stats = {
        hp = 12000,
        speed = 20,
        damage = 150,
        armor = 30,
    },
    
    phases = {
        {
            hp_percent = 1.0,
            behavior = {
                type = "flyer",
                attack_type = "breath",
                attack_range = 300,
                attack_speed = 0.3,
            },
        },
        {
            hp_percent = 0.6,
            behavior = {
                type = "grounded",  -- 落地
                attack_type = "melee",
                attack_range = 100,
                attack_speed = 0.8,
            },
        },
        {
            hp_percent = 0.2,
            behavior = {
                type = "desperate",
                attack_type = "summon_breath",
                summon_type = "skeleton_minion",
                attack_speed = 1.2,
            },
        },
    },
    
    special = {
        is_flying = true,
        can_land = true,
        breath_type = "dark_fire",
    },
    
    drops = {
        exp = 3000,
        coins = { min = 500, max = 800 },
        guaranteed = {"epic_plant_card"},
    },
}

enemies.void_lord = {
    id = "void_lord",
    name = "虛空領主",
    name_en = "Void Lord",
    description = "最終 Boss",
    tier = "boss",
    
    stats = {
        hp = 100000,
        speed = 15,
        damage = 300,
        armor = 50,
    },
    
    phases = {
        {
            hp_percent = 1.0,
            behavior = {
                type = "void_walker",
                attack_type = "void_beam",
                attack_range = 400,
                attack_speed = 0.5,
            },
        },
        {
            hp_percent = 0.75,
            behavior = {
                type = "summoner",
                summon_types = {"shadow_ghost", "shadow_assassin", "demon_guard"},
                summon_count = 5,
                summon_interval = 8.0,
            },
        },
        {
            hp_percent = 0.5,
            behavior = {
                type = "void_rift",
                -- 開啟虛空裂縫
                rift_interval = 15.0,
                rift_duration = 10.0,
                rift_spawn_rate = 2.0,
            },
        },
        {
            hp_percent = 0.25,
            behavior = {
                type = "ultimate",
                -- 全屏攻擊
                attack_type = "void_nova",
                attack_interval = 20.0,
                damage_mult = 2.0,
            },
        },
        {
            hp_percent = 0.1,
            behavior = {
                type = "desperate",
                damage_mult = 3.0,
                speed_mult = 2.0,
                armor = 0,  -- 最後階段無護甲
            },
        },
    },
    
    special = {
        invulnerable_during_phase_change = true,
        phase_change_invuln_duration = 3.0,
    },
    
    drops = {
        exp = 50000,
        coins = { min = 5000, max = 10000 },
        guaranteed = {"legendary_plant_card", "genesis_seed"},
    },
}

--------------------------------------------------------------------------------
-- 傳說敵人 (★★★★★★)
--------------------------------------------------------------------------------

enemies.skeleton_emperor = {
    id = "skeleton_emperor",
    name = "骷髏皇帝",
    name_en = "Skeleton Emperor",
    description = "無限模式 1000 層 Boss",
    tier = "legendary",
    
    stats = {
        hp = 500000,
        speed = 40,
        damage = 500,
        armor = 60,
    },
    
    -- 多階段 Boss
    phases = {
        -- ... 複雜的階段定義
    },
    
    drops = {
        exp = 500000,
        guaranteed = {"legendary_plant_fragment", "legendary_plant_fragment", "legendary_plant_fragment"},
    },
}

return enemies
