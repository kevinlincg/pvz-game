-- ============================================
-- 植物戰紀 - 植物定義
-- ============================================

local plants = {}

--------------------------------------------------------------------------------
-- 射手系
--------------------------------------------------------------------------------

plants.pea_sprite = {
    id = "pea_sprite",
    name = "豌豆精靈",
    name_en = "Pea Sprite",
    description = "基礎射手，發射豌豆攻擊敵人",
    rarity = "common",
    category = "shooter",
    
    cost = 100,
    cooldown = 5.0,
    
    stats = {
        hp = 100,
        damage = 25,
        attack_speed = 1.0,
        range = 400,
        crit_rate = 0.10,
        crit_mult = 2.0,
    },
    
    projectile = {
        type = "pea",
        speed = 500,
        color = {100, 200, 100},
    },
    
    evolution = {
        requires = {"pea_sprite", "pea_sprite", "pea_sprite"},
        evolves_to = "twin_sprite",
    },
    
    -- 攻擊行為
    on_attack = function(self, target)
        self:shoot({
            projectile = "pea",
            damage = self.stats.damage,
            speed = 500,
            on_hit = function(target)
                -- 命中效果
            end,
        })
    end,
    
    on_kill = function(self, enemy)
        game:add_combo(1)
    end,
    
    -- 動畫配置
    animations = {
        idle = { fps = 8, frames = 4 },
        attack = { fps = 12, frames = 6 },
        hurt = { fps = 10, frames = 2 },
    },
}

plants.twin_sprite = {
    id = "twin_sprite",
    name = "雙子精靈",
    name_en = "Twin Sprite",
    description = "一次發射兩顆豌豆",
    rarity = "rare",
    category = "shooter",
    
    cost = 200,
    cooldown = 5.0,
    
    stats = {
        hp = 150,
        damage = 25,
        attack_speed = 1.0,
        range = 400,
        crit_rate = 0.12,
        crit_mult = 2.0,
    },
    
    projectile = {
        type = "pea",
        count = 2,  -- 雙發
        spread = 10,  -- 角度差
        speed = 500,
    },
    
    evolution = {
        requires = {"twin_sprite", "twin_sprite", "twin_sprite"},
        evolves_to = "burst_sprite",
    },
    
    on_attack = function(self, target)
        self:shoot({
            projectile = "pea",
            count = 2,
            damage = self.stats.damage,
            speed = 500,
        })
    end,
}

plants.burst_sprite = {
    id = "burst_sprite",
    name = "連發精靈",
    name_en = "Burst Sprite",
    description = "一次發射四顆豌豆",
    rarity = "epic",
    category = "shooter",
    
    cost = 300,
    cooldown = 5.0,
    
    stats = {
        hp = 200,
        damage = 30,
        attack_speed = 1.2,
        range = 450,
        crit_rate = 0.15,
        crit_mult = 2.0,
    },
    
    projectile = {
        type = "pea",
        count = 4,
        spread = 5,
        speed = 550,
    },
    
    evolution = {
        requires = {"burst_sprite", "burst_sprite", "burst_sprite"},
        evolves_to = "gatling_sprite",
    },
}

plants.gatling_sprite = {
    id = "gatling_sprite",
    name = "機槍精靈",
    name_en = "Gatling Sprite",
    description = "一次發射八顆豌豆，攻速 +50%",
    rarity = "legendary",
    category = "shooter",
    
    cost = 500,
    cooldown = 5.0,
    
    stats = {
        hp = 300,
        damage = 40,
        attack_speed = 1.5,
        range = 500,
        crit_rate = 0.20,
        crit_mult = 2.5,
    },
    
    projectile = {
        type = "pea",
        count = 8,
        spread = 3,
        speed = 600,
    },
    
    -- 傳說植物無法進化
    evolution = nil,
}

--------------------------------------------------------------------------------
-- 火焰射手
--------------------------------------------------------------------------------

plants.blaze_sprite = {
    id = "blaze_sprite",
    name = "烈焰精靈",
    name_en = "Blaze Sprite",
    description = "發射火焰球，造成燃燒傷害",
    rarity = "rare",
    category = "shooter",
    element = "fire",
    
    cost = 175,
    cooldown = 5.0,
    
    stats = {
        hp = 120,
        damage = 35,
        attack_speed = 0.8,
        range = 350,
        crit_rate = 0.10,
    },
    
    projectile = {
        type = "fireball",
        speed = 400,
        color = {255, 100, 50},
        trail = "fire",
    },
    
    on_hit = function(self, target)
        -- 造成燃燒
        target:apply_status("burn", {
            damage = self.stats.damage * 0.1,
            duration = 3.0,
        })
    end,
}

--------------------------------------------------------------------------------
-- 冰霜射手
--------------------------------------------------------------------------------

plants.frost_sprite = {
    id = "frost_sprite",
    name = "冰霜精靈",
    name_en = "Frost Sprite",
    description = "發射冰凍球，減緩敵人速度",
    rarity = "rare",
    category = "shooter",
    element = "ice",
    
    cost = 175,
    cooldown = 5.0,
    
    stats = {
        hp = 120,
        damage = 30,
        attack_speed = 0.9,
        range = 350,
        crit_rate = 0.10,
    },
    
    projectile = {
        type = "iceball",
        speed = 450,
        color = {100, 200, 255},
        trail = "ice",
    },
    
    on_hit = function(self, target)
        -- 造成減速
        target:apply_status("slow", {
            amount = 0.5,
            duration = 2.0,
        })
        -- 有機率凍結
        if math.random() < 0.1 then
            target:apply_status("freeze", { duration = 1.0 })
        end
    end,
}

--------------------------------------------------------------------------------
-- 能量系
--------------------------------------------------------------------------------

plants.sun_bloom = {
    id = "sun_bloom",
    name = "光耀花",
    name_en = "Sun Bloom",
    description = "產生陽光能量",
    rarity = "common",
    category = "energy",
    
    cost = 50,
    cooldown = 5.0,
    
    stats = {
        hp = 80,
    },
    
    production = {
        type = "sun",
        amount = 25,
        interval = 5.0,
    },
    
    evolution = {
        requires = {"sun_bloom", "sun_bloom", "sun_bloom"},
        evolves_to = "twin_bloom",
    },
    
    on_produce = function(self)
        game:add_sun(self.production.amount)
        -- 生成視覺效果
        self:spawn_particle("sun", {
            value = self.production.amount,
            target = "player",
        })
    end,
}

plants.twin_bloom = {
    id = "twin_bloom",
    name = "雙生花",
    name_en = "Twin Bloom",
    description = "產生雙倍陽光",
    rarity = "rare",
    category = "energy",
    
    cost = 150,
    cooldown = 5.0,
    
    stats = {
        hp = 100,
    },
    
    production = {
        type = "sun",
        amount = 50,
        interval = 5.0,
    },
    
    evolution = {
        requires = {"twin_bloom", "twin_bloom", "twin_bloom"},
        evolves_to = "solar_goddess",
    },
}

plants.solar_goddess = {
    id = "solar_goddess",
    name = "太陽神花",
    name_en = "Solar Goddess",
    description = "大量產生陽光，周圍植物傷害 +10%",
    rarity = "epic",
    category = "energy",
    
    cost = 300,
    cooldown = 5.0,
    
    stats = {
        hp = 150,
    },
    
    production = {
        type = "sun",
        amount = 100,
        interval = 5.0,
    },
    
    aura = {
        range = 200,
        effect = "damage_boost",
        amount = 0.10,
    },
    
    evolution = {
        requires = {"solar_goddess", "solar_goddess", "solar_goddess"},
        evolves_to = "starlight_goddess",
    },
}

--------------------------------------------------------------------------------
-- 防禦系
--------------------------------------------------------------------------------

plants.nut_guard = {
    id = "nut_guard",
    name = "堅果衛士",
    name_en = "Nut Guard",
    description = "高血量防禦單位",
    rarity = "common",
    category = "defense",
    
    cost = 50,
    cooldown = 20.0,
    
    stats = {
        hp = 400,
        armor = 0,
        regen = 0,
    },
    
    evolution = {
        requires = {"nut_guard", "nut_guard", "nut_guard"},
        evolves_to = "iron_guard",
    },
    
    on_damage = function(self, damage)
        return damage  -- 基礎無減傷
    end,
}

plants.iron_guard = {
    id = "iron_guard",
    name = "鐵壁衛士",
    name_en = "Iron Guard",
    description = "高血量 + 20% 減傷",
    rarity = "rare",
    category = "defense",
    
    cost = 125,
    cooldown = 20.0,
    
    stats = {
        hp = 800,
        armor = 20,  -- 減傷
        regen = 0,
    },
    
    evolution = {
        requires = {"iron_guard", "iron_guard", "iron_guard"},
        evolves_to = "battle_nut",
    },
    
    on_damage = function(self, damage)
        return damage * (1 - self.stats.armor / 100)
    end,
}

plants.titan_guardian = {
    id = "titan_guardian",
    name = "泰坦守護者",
    name_en = "Titan Guardian",
    description = "超高血量 + 50% 減傷",
    rarity = "legendary",
    category = "defense",
    
    cost = 400,
    cooldown = 30.0,
    
    stats = {
        hp = 2000,
        armor = 50,
        regen = 5,  -- 每秒回 5 HP
    },
    
    on_damage = function(self, damage)
        return damage * (1 - self.stats.armor / 100)
    end,
    
    on_update = function(self, dt)
        if self.hp < self.stats.hp then
            self.hp = math.min(self.hp + self.stats.regen * dt, self.stats.hp)
        end
    end,
}

--------------------------------------------------------------------------------
-- 爆炸系
--------------------------------------------------------------------------------

plants.burst_berry = {
    id = "burst_berry",
    name = "爆裂果",
    name_en = "Burst Berry",
    description = "3x3 範圍爆炸，一次性使用",
    rarity = "common",
    category = "explosive",
    
    cost = 150,
    cooldown = 30.0,
    single_use = true,
    
    stats = {
        damage = 300,
        range = 120,  -- 1.5 cells
    },
    
    on_activate = function(self)
        -- 3x3 範圍傷害
        game:area_damage(self.x, self.y, self.stats.range, self.stats.damage)
        -- 爆炸特效
        game:spawn_effect("explosion", {
            x = self.x,
            y = self.y,
            radius = self.stats.range,
        })
        self:destroy()
    end,
}

plants.dragon_pepper = {
    id = "dragon_pepper",
    name = "火龍椒",
    name_en = "Dragon Pepper",
    description = "燒毀整行敵人，一次性使用",
    rarity = "rare",
    category = "explosive",
    
    cost = 125,
    cooldown = 30.0,
    single_use = true,
    
    stats = {
        damage = 200,
    },
    
    on_activate = function(self)
        -- 整行傷害
        game:line_damage(self.row, self.stats.damage)
        -- 火焰特效
        game:spawn_effect("fire_line", {
            row = self.row,
            duration = 1.0,
        })
        self:destroy()
    end,
}

--------------------------------------------------------------------------------
-- 陷阱系
--------------------------------------------------------------------------------

plants.mine_grass = {
    id = "mine_grass",
    name = "地雷草",
    name_en = "Mine Grass",
    description = "隱形地雷，踩到爆炸",
    rarity = "common",
    category = "trap",
    
    cost = 25,
    cooldown = 20.0,
    
    stats = {
        damage = 300,
        hidden = true,
    },
    
    on_enemy_step = function(self, enemy)
        game:area_damage(self.x, self.y, 50, self.stats.damage)
        self:destroy()
    end,
}

--------------------------------------------------------------------------------
-- 傳說植物
--------------------------------------------------------------------------------

plants.dragon_breath = {
    id = "dragon_breath",
    name = "龍息花",
    name_en = "Dragon Breath",
    description = "噴射火焰，範圍傷害",
    rarity = "legendary",
    category = "shooter",
    element = "fire",
    unlock = "prestige_50",
    
    cost = 500,
    cooldown = 5.0,
    
    stats = {
        hp = 250,
        damage = 80,
        attack_speed = 0.5,
        range = 300,
    },
    
    attack_type = "breath",  -- 噴射而非發射
    
    on_attack = function(self)
        -- 錐形範圍傷害
        game:cone_damage(self.x, self.y, 60, self.stats.range, self.stats.damage)
        -- 燃燒效果
        for _, enemy in ipairs(game:get_enemies_in_cone(self.x, self.y, 60, self.stats.range)) do
            enemy:apply_status("burn", {
                damage = self.stats.damage * 0.15,
                duration = 4.0,
            })
        end
    end,
}

plants.phoenix_bloom = {
    id = "phoenix_bloom",
    name = "鳳凰花",
    name_en = "Phoenix Bloom",
    description = "死亡後復活一次",
    rarity = "legendary",
    category = "energy",
    element = "fire",
    unlock = "damage_10000000",
    
    cost = 400,
    cooldown = 5.0,
    
    stats = {
        hp = 200,
        damage = 50,
        attack_speed = 1.0,
    },
    
    production = {
        type = "sun",
        amount = 75,
        interval = 4.0,
    },
    
    revive_count = 1,
    
    on_death = function(self)
        if self.revive_count > 0 then
            self.revive_count = self.revive_count - 1
            self.hp = self.stats.hp
            -- 復活特效
            game:spawn_effect("revive", { x = self.x, y = self.y })
            return false  -- 不真正死亡
        end
        return true  -- 真正死亡
    end,
}

return plants
