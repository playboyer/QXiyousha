--hulu
local hulu_skill={}
hulu_skill.name="hulu"
table.insert(sgs.ai_skills,hulu_skill)
hulu_skill.getTurnUseCard=function(self)
    if self.player:isKongcheng() then return end
	if not self.player:hasUsed("HuluCard") then
		return sgs.Card_Parse("@HuluCard=.")
	end
end

sgs.ai_skill_use_func["HuluCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
	if not enemy:isKongcheng() then
			use.card=card
			if use.to then use.to:append(enemy) end
			return
	   end
	end
end

--qiuyang
local qiuyang_skill={}
qiuyang_skill.name="qiuyang"
table.insert(sgs.ai_skills,qiuyang_skill)
qiuyang_skill.getTurnUseCard=function(self)
	if not self.player:hasUsed("QiuyangCard") then
		return sgs.Card_Parse("@QiuyangCard=.")
	end
end

sgs.ai_skill_use_func["QiuyangCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
	if not enemy:isKongcheng() and enemy:getGeneral():isMale() then
			use.card=card
			if use.to then use.to:append(enemy) end
			return
	   end
	end
end

local wuzhishan_skill={}
wuzhishan_skill.name="wuzhishan"
table.insert(sgs.ai_skills,wuzhishan_skill)
wuzhishan_skill.getTurnUseCard=function(self,inclusive)
    local cards = self.player:getCards("he")
    cards=sgs.QList2Table(cards)

	local card

	self:sortByUseValue(cards,true)

	local has_weapon=false

	for _,acard in ipairs(cards)  do
	    if acard:inherits("Weapon") and not (acard:getSuit() == sgs.Card_Diamond) then has_weapon=true end
	end

	for _,acard in ipairs(cards)  do
		if (acard:getSuit() == sgs.Card_Diamond) and ((self:getUseValue(acard)<sgs.ai_use_value["Indulgence"]) or inclusive) then
		    local shouldUse=true

		    if acard:inherits("Armor") then
                if not self.player:getArmor() then shouldUse=false
                elseif self:hasEquip(acard) then shouldUse=false
                end
            end

            if acard:inherits("Weapon") then
                if not self.player:getWeapon() then shouldUse=false
                elseif self:hasEquip(acard) and not has_weapon then shouldUse=false
                end
            end

		    if shouldUse then
			    card = acard
			    break
			end
		end
	end

	    if not card then return nil end
		local number = card:getNumberString()
	    local card_id = card:getEffectiveId()
		local card_str = ("indulgence:wuzhishan[diamond:%s]=%d"):format(number, card_id)
		local indulgence = sgs.Card_Parse(card_str)

	    assert(indulgence)

        return indulgence

end

--guanghan
sgs.ai_skill_use["@guanghan"]=function(self, data)
	local card_id
	local cant_use_skill

	if data=="@@guanghan-card" then
		dmg = self.player:getTag("GuanghanDamage"):toDamage()
	else
		dmg = data
	end

	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards,true)
	for _,card in ipairs(cards) do
		if card:isRed() and not card:inherits("Peach") then
			card_id = card:getId()
			break
		end
	end
	if not card_id then return "." end

	for _, enemy in ipairs(self.enemies) do
		if self.player:canSlash(enemy,true) and not enemy:hasFlag("damage_source")
		then return "@GuanghanCard="..card_id.."->"..enemy:objectName() end
	end
	return "."
end

--jingtan
local jingtan_skill={}
jingtan_skill.name="jingtan"
table.insert(sgs.ai_skills,jingtan_skill)
jingtan_skill.getTurnUseCard=function(self)
	if not self.player:hasUsed("JingtanCard") then
		return sgs.Card_Parse("@JingtanCard=.")
	end
end

sgs.ai_skill_use_func["JingtanCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
	if self:getCardsNum(".", enemy, "e") >= 2 and self.player:getHp() > 2 then
			use.card=card
			if use.to then use.to:append(enemy) end
			return
	   end
	end
end

local wusheng_skill={}
wusheng_skill.name="wusheng"
table.insert(sgs.ai_skills,wusheng_skill)
wusheng_skill.getTurnUseCard=function(self,inclusive)
    local cards = self.player:getCards("h")
    cards=sgs.QList2Table(cards)

	local black_card

	self:sortByUseValue(cards,true)

	for _,card in ipairs(cards)  do
		if card:isBlack() and not card:inherits("Slash") and not card:inherits("Peach") 				--not peach
			and ((self:getUseValue(card)<sgs.ai_use_value["Slash"]) or inclusive) then
			black_card = card
			break
		end
	end

	if black_card then
		local suit = black_card:getSuitString()
    	local number = black_card:getNumberString()
		local card_id = black_card:getEffectiveId()
		local card_str = ("slash:wusheng[%s:%s]=%d"):format(suit, number, card_id)
		local slash = sgs.Card_Parse(card_str)
		slash:setObjectName("thunder_slash")

		assert(slash)

        return slash
	end
end

local longdan_skill={}
longdan_skill.name="longdan"
table.insert(sgs.ai_skills,longdan_skill)
longdan_skill.getTurnUseCard=function(self)
    local cards = self.player:getCards("h")
    cards=sgs.QList2Table(cards)

	local jink_card

	self:sortByUseValue(cards,true)

	for _,card in ipairs(cards)  do
		if card:inherits("Jink") then
			jink_card = card
			break
		end
	end

	    if not jink_card then return nil end
		local suit = jink_card:getSuitString()
		local number = jink_card:getNumberString()
		local card_id = jink_card:getEffectiveId()
		local card_str = ("slash:longdan[%s:%s]=%d"):format(suit, number, card_id)
		local slash = sgs.Card_Parse(card_str)
		slash:setObjectName("thunder_slash")
	    assert(slash)

        return slash

end

local lijian_skill={}
lijian_skill.name="lijian"
table.insert(sgs.ai_skills,lijian_skill)
lijian_skill.getTurnUseCard=function(self)
	if self.player:hasUsed("LijianCard") then
		return
	end
	if not self.player:isKongcheng() then
		local card
		local card_id
		if self.player:getHandcardNum() > self.player:getHp() then
			local cards = self.player:getHandcards()
			cards=sgs.QList2Table(cards)

			for _, acard in ipairs(cards) do
				if (acard:inherits("BasicCard") or acard:inherits("EquipCard") or acard:inherits("AmazingGrace"))
					and not acard:inherits("Peach") and not acard:inherits("Shit") then
					card_id = acard:getEffectiveId()
					break
				end
			end
		end
		if not card_id then
			cards=sgs.QList2Table(self.player:getHandcards())
			for _, acard in ipairs(cards) do
				if (acard:inherits("BasicCard") or acard:inherits("EquipCard") or acard:inherits("AmazingGrace"))
					and not acard:inherits("Peach") and not acard:inherits("Shit") then
					card_id = acard:getEffectiveId()
					break
				end
			end
		end
		if not card_id then
			return nil
		else
			card = sgs.Card_Parse("@LijianCard=" .. card_id)
			return card
		end
	end
	return nil
end

sgs.ai_skill_use_func["LijianCard"]=function(card,use,self)
	local findFriend_maxSlash=function(self,first)
		self:log("Looking for the friend!")
		local maxSlash = 0
		local friend_maxSlash
		for _, friend in ipairs(self.friends_noself) do
			if (self:getCardsNum("Slash", friend)> maxSlash) then
				maxSlash=self:getCardsNum("Slash", friend)
				friend_maxSlash = friend
			end
		end
		if friend_maxSlash then
			local safe = false
			if (first:hasSkill("ganglie") or first:hasSkill("fankui") or first:hasSkill("enyuan")) then
				if (first:getHp()<=1 and first:getHandcardNum()==0) then safe=true end
			elseif (self:getCardsNum("Slash", friend_maxSlash) >= first:getHandcardNum()) then safe=true end
			if safe then return friend_maxSlash end
		else self:log("unfound")
		end
		return nil
	end

	if not self.player:hasUsed("LijianCard") then
		self:sort(self.enemies, "hp")
		local males = {}
		local first, second
		local zhugeliang_kongcheng
		for _, enemy in ipairs(self.enemies) do
			if zhugeliang_kongcheng and #males==1 then table.insert(males, zhugeliang_kongcheng) end
			if not enemy:hasSkill("wuyan") then
				if enemy:hasSkill("kongcheng") and enemy:isKongcheng() then	zhugeliang_kongcheng=enemy
				else table.insert(males, enemy)	end
				if #males >= 2 then	break end
			end
		end
		if (#males==1) and #self.friends_noself>0 then
			self:log("Only 1")
			first = males[1]
			if zhugeliang_kongcheng then table.insert(males, zhugeliang_kongcheng)
			else
				local friend_maxSlash = findFriend_maxSlash(self,first)
				if friend_maxSlash then table.insert(males, friend_maxSlash) end
			end
		end
		if (#males >= 2) then
			first = males[1]
			second = males[2]
			local lord = self.room:getLord()
			if (first:getHp()<=1) then
				if self.player:isLord() or isRolePredictable() then
					local friend_maxSlash = findFriend_maxSlash(self,first)
					if friend_maxSlash then second=friend_maxSlash end
				elseif (lord:getGeneral():isMale()) and (not lord:hasSkill("wuyan")) then
					if (self.role=="rebel") and (not first:isLord()) then
						second = lord
					else
						if ((self.role=="loyalist" or (self.role=="renegade") and not (first:hasSkill("ganglie") and first:hasSkill("enyuan"))))
							and	( self:getCardsNum("Slash", first)<=self:getCardsNum("Slash", second)) then
							second = lord
						end
					end
				end
			end

			if first and second
					then
				if use.to
					then
		        use.to:append(first)
		        use.to:append(second)
				end
			end
            use.card=card
		end
	end
end

--qishier
local qishier_skill={}
qishier_skill.name="qishier"
table.insert(sgs.ai_skills,qishier_skill)
qishier_skill.getTurnUseCard=function(self)
	local cards = self.player:getHandcards()
	local reds = {}
	for _, card in sgs.qlist(cards) do
		if card:isRed() and not card:inherits("Peach") then
			table.insert(reds, card)
		end
	end

	if #reds == 0 or self.player:hasUsed("QishierCard") then return end
	self:sortByUseValue(reds, true)

	local card_str = ("@QishierCard=%d"):format(reds[1]:getEffectiveId())
	return sgs.Card_Parse(card_str)
end

sgs.ai_skill_use_func["QishierCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
	if (enemy:hasSkill("lijian")
		or enemy:hasSkill("hulu")
		or enemy:hasSkill("qiuyang")
		or enemy:hasSkill("fanjian")
		or enemy:hasSkill("qingnang")) and self.player:getHandcardNum() >= 2
		then
			use.card=card
			if use.to then use.to:append(enemy) end
			return
		end
	end
    for _, friend in ipairs(self.friends_noself) do
	if (friend:hasSkill("lijian")
		or friend:hasSkill("hulu")
		or friend:hasSkill("qiuyang")
		or friend:hasSkill("fanjian")
		or friend:hasSkill("qingnang")) and self.player:getHandcardNum() >= 2
		then
			use.card=card
			if use.to then use.to:append(friend) end
			return
		end
	end
end

--yjqishier
local yjqishier_skill={}
yjqishier_skill.name="yjqishier"
table.insert(sgs.ai_skills,yjqishier_skill)
yjqishier_skill.getTurnUseCard=function(self)
	local cards = self.player:getHandcards()
	local reds = {}
	for _, card in sgs.qlist(cards) do
		if card:isRed() and not card:inherits("Peach") then
			table.insert(reds, card)
		end
	end

	if #reds == 0 or self.player:hasUsed("YjqishierCard") then return end
	self:sortByUseValue(reds, true)

	local card_str = ("@YjqishierCard=%d"):format(reds[1]:getEffectiveId())
	return sgs.Card_Parse(card_str)
end

sgs.ai_skill_use_func["YjqishierCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
	if (enemy:hasSkill("lijian")
		or enemy:hasSkill("hulu")
		or enemy:hasSkill("qiuyang")
		or enemy:hasSkill("fanjian")
		or enemy:hasSkill("qingnang")) and self.player:getHandcardNum() >= 2
		then
			use.card=card
			if use.to then use.to:append(enemy) end
			return
		end
	end
    for _, friend in ipairs(self.friends_noself) do
	if (friend:hasSkill("lijian")
		or friend:hasSkill("hulu")
		or friend:hasSkill("qiuyang")
		or friend:hasSkill("fanjian")
		or friend:hasSkill("qingnang")) and self.player:getHandcardNum() >= 2
		then
			use.card=card
			if use.to then use.to:append(friend) end
			return
		end
	end
end

--shengsibu
local xingshang_skill={}
xingshang_skill.name="xingshang"
table.insert(sgs.ai_skills,xingshang_skill)
xingshang_skill.getTurnUseCard=function(self)
	local cards = self.player:getHandcards()
	local shengsies = {}
	for _, card in sgs.qlist(cards) do
		if card:inherits("Lightning") then
			table.insert(shengsies, card)
		end
	end

	if #shengsies == 0 then return end
	self:sortByUseValue(shengsies, true)

	local card_str = ("@XingshangCard=%d"):format(shengsies[1]:getEffectiveId())
	return sgs.Card_Parse(card_str)
end

sgs.ai_skill_use_func["XingshangCard"]=function(card,use,self)
	for _, enemy in ipairs(self.enemies) do
			use.card=card
			if use.to then use.to:append(enemy) end
			return
	end
end

-- jiuku
sgs.ai_skill_invoke.jiuku = function(self, data)
	local damage = data:toDamage()
	local cards = self.player:getHandcards()
	local reds = {}
	for _, card in sgs.qlist(cards) do
		if card:isRed() and not card:inherits("Peach") then
			table.insert(reds, card)
		end
	end

	if #reds == 0 then return end

	return self:isFriend(damage.to) and not self:isFriend(damage.from) and damage.to~=self.player
end

--bajiaoshan
sgs.ai_skill_invoke.bajiaoshan = function(self, data)
	local damage = data:toDamage()
	return self:isEnemy(damage.to) and damage.to:getNextAlive()~=self.player
end

--ruyibang
sgs.ai_skill_invoke.ruyibang = function(self, data)
	local effect = data:toSlashEffect()
	local target = effect.to
	return self:isEnemy(effect.to:getNextAlive())
end

--dingfeng
sgs.ai_skill_invoke.dingfeng = function(self, data)
	local effect = data:toCardEffect()
	if effect.card:inherits("GodSalvation") or effect.card:inherits("AmazingGrace") then return end
	return self:isEnemy(effect.from) or effect.card:inherits("AOE")
end

sgs.ai_skill_choice.sihai = function(self, choices)
	return "red"
end
