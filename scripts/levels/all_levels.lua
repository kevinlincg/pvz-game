-- ============================================
-- 植物戰紀 - 關卡定義
-- ============================================

levels = {}

--------------------------------------------------------------------------------
-- 區域 1: 萌芽平原 (關卡 1-100)
--------------------------------------------------------------------------------

levels["1-1"] = {
    id = "1-1",
    name = "萌芽平原 1-1",
    name_en = "Sprouting Plains 1-1",
    area = "sprouting_plains",
    area_name = "萌芽平原",
    
    theme = {
        background = "grassland_day",
        music = "peaceful_meadow",
        weather = "clear",
    },
    
    grid = {
        cols = 9,
        rows = 5,
    },
    
    -- 初始狀態
    initial = {
        sun = 50,
        plants = {},  -- 無預設植物
    },
    
    -- 可用植物
    unlocked_plants = {"pea_sprite"},
    
    -- 波次定義
    waves = {
        { 
            time = 0, 
            enemies = {
                { type = "corrupted_slime", count = 3, interval = 2.0 }
            }
        },
        { 
            time = 15, 
            enemies = {
                { type = "corrupted_slime", count = 2, interval = 2.0 },
                { type = "skeleton_minion", count = 1, interval = 0 }
            }
        },
        { 
            time = 30, 
            enemies = {
                { type = "skeleton_minion", count = 3, interval = 3.0 }
            }
        },
    },
    
    -- 三星條件
    stars = {
        { id = "complete", name = "通關", reward = { exp = 50, coins = 20 } },
        { id = "no_plant_lost", name = "無植物損失", reward = { exp = 25, coins = 10 } },
        { id = "under_time", name = "60秒內通關", time = 60, reward = { exp = 25, coins = 10 } },
    },
    
    -- 通關獎勵
    completion_reward = {
        exp = 100,
        coins = 30,
        unlock_next = "1-2",
    },
    
    -- 教學提示
    tutorial = {
        { time = 0, text = "歡迎來到植物戰紀！" },
        { time = 2, text = "點擊左側的豌豆精靈，然後點擊草地放置" },
        { time = 5, text = "豌豆精靈會自動攻擊靠近的敵人" },
    },
}

levels["1-2"] = {
    id = "1-2",
    name = "萌芽平原 1-2",
    name_en = "Sprouting Plains 1-2",
    area = "sprouting_plains",
    
    theme = {
        background = "grassland_day",
        music = "peaceful_meadow",
        weather = "clear",
    },
    
    initial = {
        sun = 50,
        plants = {},
    },
    
    unlocked_plants = {"pea_sprite", "sun_bloom"},
    
    waves = {
        { 
            time = 0, 
            enemies = {
                { type = "corrupted_slime", count = 5, interval = 2.0 }
            }
        },
        { 
            time = 20, 
            enemies = {
                { type = "corrupted_slime", count = 3, interval = 1.5 },
                { type = "skeleton_minion", count = 2, interval = 3.0 }
            }
        },
        { 
            time = 40, 
            enemies = {
                { type = "skeleton_minion", count = 4, interval = 2.0 }
            }
        },
    },
    
    stars = {
        { id = "complete", name = "通關", reward = { exp = 50, coins = 20 } },
        { id = "no_plant_lost", name = "無植物損失", reward = { exp = 25, coins = 10 } },
        { id = "under_time", name = "60秒內通關", time = 60, reward = { exp = 25, coins = 10 } },
    },
    
    completion_reward = {
        exp = 120,
        coins = 35,
        unlock_next = "1-3",
    },
    
    tutorial = {
        { time = 0, text = "你需要更多陽光來種植更多植物" },
        { time = 2, text = "種植光耀花可以產生陽光！" },
    },
}

levels["1-3"] = {
    id = "1-3",
    name = "萌芽平原 1-3",
    name_en = "Sprouting Plains 1-3",
    area = "sprouting_plains",
    
    theme = {
        background = "grassland_day",
        music = "peaceful_meadow",
        weather = "clear",
    },
    
    initial = {
        sun = 100,
        plants = {},
    },
    
    unlocked_plants = {"pea_sprite", "sun_bloom", "nut_guard"},
    
    waves = {
        { time = 0, enemies = { { type = "corrupted_slime", count = 6, interval = 1.5 } } },
        { time = 15, enemies = { { type = "skeleton_minion", count = 4, interval = 2.0 } } },
        { time = 30, enemies = { { type = "corrupted_slime", count = 4, interval = 1.0 }, { type = "skeleton_minion", count = 3, interval = 2.0 } } },
        { time = 50, enemies = { { type = "skeleton_minion", count = 5, interval = 1.5 } } },
    },
    
    stars = {
        { id = "complete", name = "通關", reward = { exp = 50, coins = 20 } },
        { id = "no_plant_lost", name = "無植物損失", reward = { exp = 25, coins = 10 } },
        { id = "under_time", name = "70秒內通關", time = 70, reward = { exp = 25, coins = 10 } },
    },
    
    completion_reward = {
        exp = 150,
        coins = 40,
        unlock_next = "1-4",
    },
    
    tutorial = {
        { time = 0, text = "敵人越來越多了！" },
        { time = 2, text = "種植堅果衛士可以擋住敵人" },
    },
}

--------------------------------------------------------------------------------
-- Boss 關卡
--------------------------------------------------------------------------------

levels["1-10"] = {
    id = "1-10",
    name = "萌芽平原 1-10 - Boss",
    name_en = "Sprouting Plains 1-10 - Boss",
    area = "sprouting_plains",
    is_boss = true,
    
    theme = {
        background = "grassland_sunset",
        music = "boss_battle_1",
        weather = "ominous",
    },
    
    initial = {
        sun = 200,
        plants = {},
    },
    
    unlocked_plants = {"pea_sprite", "sun_bloom", "nut_guard", "twin_sprite", "blaze_sprite"},
    
    -- Boss 波次
    waves = {
        { 
            time = 0, 
            enemies = {
                { type = "skeleton_minion", count = 5, interval = 1.0 }
            }
        },
        { 
            time = 10, 
            boss = {
                type = "skeleton_king",
                spawn_time = 15,
            },
            enemies = {
                { type = "skeleton_minion", count = 3, interval = 5.0 }
            }
        },
    },
    
    stars = {
        { id = "complete", name = "擊敗骷髏王", reward = { exp = 200, coins = 100 } },
        { id = "no_plant_lost", name = "無植物損失", reward = { exp = 100, coins = 50 } },
        { id = "under_time", name = "120秒內通關", time = 120, reward = { exp = 100, coins = 50 } },
    },
    
    completion_reward = {
        exp = 1000,
        coins = 300,
        unlock_next = "2-1",
        unlock_area = "dark_forest",
        guaranteed_drop = { type = "rare_plant_card", plant = "blaze_sprite" },
    },
    
    -- Boss 登場動畫
    boss_intro = {
        { time = 0, text = "敵人的腳步聲越來越近..." },
        { time = 3, text = "骷髏王出現了！" },
        { time = 5, text = "準備戰鬥！" },
    },
}

--------------------------------------------------------------------------------
-- 區域 2: 幽暗森林 (關卡 101-200)
--------------------------------------------------------------------------------

levels["2-1"] = {
    id = "2-1",
    name = "幽暗森林 2-1",
    name_en = "Dark Forest 2-1",
    area = "dark_forest",
    area_name = "幽暗森林",
    
    theme = {
        background = "dark_forest_night",
        music = "spooky_forest",
        weather = "fog",
        fog_density = 0.3,
    },
    
    initial = {
        sun = 150,
        plants = {},
    },
    
    unlocked_plants = {"pea_sprite", "sun_bloom", "nut_guard", "twin_sprite", "blaze_sprite", "frost_sprite"},
    
    waves = {
        { time = 0, enemies = { { type = "shadow_ghost", count = 3, interval = 2.0 } } },
        { time = 20, enemies = { { type = "skeleton_minion", count = 4, interval = 2.0 }, { type = "shadow_ghost", count = 2, interval = 3.0 } } },
        { time = 40, enemies = { { type = "shadow_ghost", count = 5, interval = 1.5 }, { type = "skeleton_archer", count = 2, interval = 4.0 } } },
    },
    
    stars = {
        { id = "complete", name = "通關", reward = { exp = 75, coins = 30 } },
        { id = "no_plant_lost", name = "無植物損失", reward = { exp = 40, coins = 20 } },
        { id = "under_time", name = "70秒內通關", time = 70, reward = { exp = 40, coins = 20 } },
    },
    
    completion_reward = {
        exp = 200,
        coins = 50,
        unlock_next = "2-2",
    },
    
    -- 區域特色介紹
    area_intro = {
        { time = 0, text = "你進入了幽暗森林..." },
        { time = 2, text = "這裡的暗影幽靈可以穿過植物！" },
        { time = 4, text = "小心它們偷襲你的後排！" },
    },
}

--------------------------------------------------------------------------------
-- 無限模式配置
--------------------------------------------------------------------------------

levels.endless = {
    id = "endless",
    name = "無限模式",
    name_en = "Endless Mode",
    
    -- 基礎配置
    base_config = {
        grid = { cols = 9, rows = 5 },
        initial_sun = 200,
        starting_plants = {"pea_sprite", "sun_bloom", "nut_guard"},
    },
    
    -- 難度成長
    difficulty = {
        -- 每層敵人 HP 增長
        hp_growth = 1.05,
        -- 每 10 層敵人速度增長
        speed_growth = 1.02,
        -- 每層生成間隔縮短
        spawn_interval_decay = 0.98,
        min_spawn_interval = 0.5,
    },
    
    -- 波次生成規則
    wave_generation = {
        -- 基礎敵人池（根據層數解鎖更多）
        enemy_pools = {
            { min_floor = 1, max_floor = 50, enemies = {"corrupted_slime", "skeleton_minion"} },
            { min_floor = 51, max_floor = 150, enemies = {"corrupted_slime", "skeleton_minion", "shadow_ghost", "bucket_skeleton"} },
            { min_floor = 151, max_floor = 500, enemies = {"skeleton_minion", "bucket_skeleton", "berserk_werewolf", "skeleton_archer"} },
            { min_floor = 501, max_floor = 1000, enemies = {"bucket_skeleton", "dark_knight", "shadow_assassin", "giant_zombie"} },
            { min_floor = 1001, enemies = "all" },
        },
        
        -- Boss 出現規則
        boss_floors = {
            { floor = 10, boss = "skeleton_king" },
            { floor = 50, boss = "voodoo_mage" },
            { floor = 100, boss = "bone_dragon" },
            { floor = 500, boss = "void_lord" },
            { floor = 1000, boss = "skeleton_emperor" },
            -- 之後每 100 層一個 Boss
        },
    },
    
    -- 獎勵
    rewards = {
        floor_100 = { exp = 5000, coins = 1000, unlock = "burst_sprite" },
        floor_500 = { exp = 50000, coins = 10000, unlock = "gatling_sprite" },
        floor_1000 = { exp = 500000, coins = 100000, unlock = "dragon_breath" },
        floor_5000 = { exp = 5000000, coins = 1000000, unlock = "phoenix_bloom" },
        floor_10000 = { exp = 50000000, coins = 10000000, unlock = "genesis_flower" },
    },
    
    -- 重生門檻
    prestige = {
        min_floor = 100,
        recommended_floors = {100, 500, 1000, 2000, 5000, 10000},
    },
}

return levels
