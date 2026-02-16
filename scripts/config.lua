-- ============================================
-- Plant Legends - 核心配置
-- ============================================

config = {
    -- 遊戲資訊
    game = {
        name = "植物戰紀",
        version = "0.1.0",
        author = "Kevin",
    },
    
    -- 全局數值
    global = {
        -- 初始資源
        starting_sun = 50,
        sun_generation_interval = 5.0,  -- 秒
        
        -- 網格
        grid_cols = 9,
        grid_rows = 5,
        cell_width = 80,
        cell_height = 100,
        
        -- Combo
        combo_timeout = 3.0,  -- 秒
        combo_multipliers = {
            [5] = 1.2,
            [10] = 1.5,
            [25] = 2.0,
            [50] = 3.0,
            [100] = 5.0,
            [200] = 10.0,
        },
        
        -- 爆擊
        crit_multiplier = 2.0,
        mega_crit_multiplier = 5.0,
        legendary_crit_multiplier = 100.0,
        
        -- 經驗值
        exp_to_level_base = 100,
        exp_to_level_growth = 1.5,  -- 每級乘數
    },
    
    -- 稀有度加成
    rarity = {
        common   = { mult = 1.0, color = {200, 200, 200} },
        rare     = { mult = 1.2, color = {100, 150, 255} },
        epic     = { mult = 1.5, color = {180, 100, 255} },
        legendary = { mult = 2.0, color = {255, 200, 50} },
    },
    
    -- 進化
    evolution = {
        fuse_count = 3,  -- 融合需要的數量
        evolve_bonus = 1.5,  -- 進化後屬性加成
    },
    
    -- 元素反應
    elements = {
        fire = {
            burn_damage = 0.1,  -- 每秒 10% 攻擊力
            burn_duration = 3.0,
        },
        ice = {
            slow_amount = 0.5,
            slow_duration = 2.0,
            freeze_chance = 0.1,
        },
        lightning = {
            chain_count = 3,
            chain_damage_decay = 0.7,
        },
        poison = {
            dot_damage = 0.05,
            dot_duration = 5.0,
            stack_max = 5,
        },
    },
    
    -- 重生系統
    prestige = {
        min_level = 100,
        base_bonus = 0.10,  -- 每次重生 +10%
        bonus_growth = {
            [10] = 0.15,
            [50] = 0.20,
            [100] = 0.25,
        },
    },
    
    -- 無限模式
    endless = {
        base_enemy_hp = 100,
        hp_growth_per_wave = 1.1,  -- 每波 +10% HP
        spawn_rate_growth = 0.95,  -- 生成間隔 x0.95
    },
}

return config
