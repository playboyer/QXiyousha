sgs.ai_skill_playerchosen.zero_card_as_slash = function(self, targets)
	local slash = sgs.Sanguosha:cloneCard("slash", sgs.Card_NoSuit, 0)
	local targetlist=sgs.QList2Table(targets)
	self:sort(targetlist, "defense")
	for _, target in ipairs(targetlist) do
		if self:isEnemy(target) and not self:slashProhibit(slash ,target) and self:slashIsEffective(slash,target) then
			return target
		end
	end
	for i=#targetlist, 1, -1 do
		if not self:slashProhibit(slash, targetlist[i]) then
			return targetlist[i]
		end
	end
	return targets:first()
end

sgs.ai_skill_playerchosen.damage = function(self, targets)
	local targetlist=sgs.QList2Table(targets)
	self:sort(targetlist,"hp")
	for _, target in ipairs(targetlist) do
		if self:isEnemy(target) then return target end
	end
	return targetlist[#targetlist]
end

sgs.ai_skill_invoke.ice_sword=function(self, data)
	if self.player:hasFlag("drank") then return false end
	local effect = data:toSlashEffect()
	local target = effect.to
	if self:isFriend(target) then return false end
	local hasPeach
	local cards = target:getHandcards()
	for _, card in sgs.qlist(cards) do
		if card:inherits("Peach") or card:inherits("Analeptic") then hasPeach = true break end
	end
	if hasPeach then return true end
	if (target:getHandcardNum() > 1 or target:getArmor()) and target:getHp() > 1 then
		return true
	end
	return false
end

sgs.ai_skill_cardchosen.ice_sword = function(self, who)
	local hcards = who:getCards("h")
	hcards = sgs.QList2Table(hcards)
	for _, peach in ipairs(hcards) do
		if peach:inherits("Peach") or peach:inherits("Analeptic") then return peach end
	end
end

local spear_skill={}
spear_skill.name="spear"
table.insert(sgs.ai_skills,spear_skill)
spear_skill.getTurnUseCard=function(self,inclusive)
    local cards = self.player:getCards("h")
    cards=sgs.QList2Table(cards)

    if #cards<(self.player:getHp()+1) then return nil end
    if #cards<2 then return nil end
    if self:getCard("Slash") then return nil end

    self:sortByUseValue(cards,true)

    local suit1 = cards[1]:getSuitString()
	local card_id1 = cards[1]:getEffectiveId()

	local suit2 = cards[2]:getSuitString()
	local card_id2 = cards[2]:getEffectiveId()

	local suit="no_suit"
	if cards[1]:isBlack() == cards[2]:isBlack() then suit = suit1 end

	local card_str = ("slash:spear[%s:%s]=%d+%d"):format(suit, 0, card_id1, card_id2)

    local slash = sgs.Card_Parse(card_str)
	slash:setObjectName("fire_slash")

    return slash

end

local qixi_skill={}
qixi_skill.name="qixi"
table.insert(sgs.ai_skills,qixi_skill)
qixi_skill.getTurnUseCard=function(self,inclusive)
    local cards = self.player:getCards("he")
    cards=sgs.QList2Table(cards)

	local black_card

	self:sortByUseValue(cards,true)

	local has_weapon=false

	for _,card in ipairs(cards)  do
	    if card:inherits("Weapon") and card:isRed() then has_weapon=true end
	end

	for _,card in ipairs(cards)  do
		if card:isBlack()  and ((self:getUseValue(card)<sgs.ai_use_value["Dismantlement"]) or inclusive or self:getOverflow()>0) then
		    local shouldUse=true

		    if card:inherits("Armor") then
                if not self.player:getArmor() then shouldUse=false
                elseif self:hasEquip(card) and not (card:inherits("SilverLion") and self.player:isWounded()) then shouldUse=false
                end
            end

            if card:inherits("Weapon") then
                if not self.player:getWeapon() then shouldUse=false
                elseif self:hasEquip(card) and not has_weapon and not card:inherits("YitianSword") then shouldUse=false
                end
            end

		    if shouldUse then
			    black_card = card
			    break
			end

		end
	end

	if black_card then
		local suit = black_card:getSuitString()
		local number = black_card:getNumberString()
		local card_id = black_card:getEffectiveId()
		local card_str = ("dismantlement:qixi[%s:%s]=%d"):format(suit, number, card_id)
		local dismantlement = sgs.Card_Parse(card_str)

		assert(dismantlement)

        return dismantlement
	end
end

local fanjian_skill={}
fanjian_skill.name="fanjian"
table.insert(sgs.ai_skills,fanjian_skill)
fanjian_skill.getTurnUseCard=function(self)
        if self.player:isKongcheng() then return nil end
        if self.player:usedTimes("FanjianCard")>0 then return nil end

		local cards = self.player:getHandcards()

		for _, card in sgs.qlist(cards) do
			if card:getSuit() == sgs.Card_Diamond and self.player:getHandcardNum() == 1 then
				return nil
			elseif card:inherits("Peach") or card:inherits("Analeptic") then
				return nil
			end
		end


		local card_str = "@FanjianCard=."
		local fanjianCard = sgs.Card_Parse(card_str)
	    assert(fanjianCard)

        return fanjianCard

end

sgs.ai_skill_use_func["FanjianCard"]=function(card,use,self)
	self:sort(self.enemies, "hp")

	for _, enemy in ipairs(self.enemies) do
		if (enemy:getGeneral():isMale() and not enemy:hasSkill("qingnang")) or (enemy:getGeneral():isMale() and enemy:getHp() == 1 and enemy:getHandcardNum() == 0 and not enemy:getEquips()) then
			use.card = card
			if use.to then use.to:append(enemy) end
			return
		end
	end
end

local jieyin_skill={}
jieyin_skill.name="jieyin"
table.insert(sgs.ai_skills,jieyin_skill)
jieyin_skill.getTurnUseCard=function(self)
        if self.player:getHandcardNum()<2 then return nil end
        if self.player:hasUsed("JieyinCard") then return nil end

		local cards = self.player:getHandcards()
		cards=sgs.QList2Table(cards)

		local first, second
		self:sortByUseValue(cards,true)
		for _, card in ipairs(cards) do
			if card:getTypeId() ~= sgs.Card_Equip then
				if not first then first  = cards[1]:getEffectiveId()
				else second = cards[2]:getEffectiveId()
				end
			end
			if second then break end
		end

		if not second then return end
		local card_str = ("@JieyinCard=%d+%d"):format(first, second)
		assert(card_str)
		return sgs.Card_Parse(card_str)
end

sgs.ai_skill_use_func["JieyinCard"]=function(card,use,self)
	self:sort(self.friends, "hp")

	for _, friend in ipairs(self.friends) do
		if friend:getGeneral():isMale() and friend:isWounded() then
			use.card=card
			if use.to then use.to:append(friend) end
			return
		end
	end
end

local qingnang_skill={}
qingnang_skill.name="qingnang"
table.insert(sgs.ai_skills,qingnang_skill)
qingnang_skill.getTurnUseCard=function(self)
        if self.player:getHandcardNum()<1 then return nil end
        if self.player:usedTimes("QingnangCard")>0 then return nil end

		local cards = self.player:getHandcards()
		cards=sgs.QList2Table(cards)

		self:sortByKeepValue(cards)

		local card_str = ("@QingnangCard=%d"):format(cards[1]:getId())
		return sgs.Card_Parse(card_str)
end

sgs.ai_skill_use_func["QingnangCard"]=function(card,use,self)
	self:sort(self.friends, "defense")

	for _, friend in ipairs(self.friends) do
		if friend:isWounded() then
			use.card=card
			if use.to then use.to:append(friend) end
			return
		end
	end
end

local kurou_skill={}
kurou_skill.name="kurou"
table.insert(sgs.ai_skills,kurou_skill)
kurou_skill.getTurnUseCard=function(self,inclusive)
        if  (self.player:getHp() > 3 and self.player:getHandcardNum() > self.player:getHp()) or
		(self.player:getHp() - self.player:getHandcardNum() >= 2) then
                return sgs.Card_Parse("@KurouCard=.")
        end

		--if not inclusive then return nil end

	if self.player:getWeapon() and self.player:getWeapon():inherits("Crossbow") then
        for _, enemy in ipairs(self.enemies) do
            if self.player:canSlash(enemy,true) and self.player:getHp()>1 then
                return sgs.Card_Parse("@KurouCard=.")
            end
        end
    end
end

sgs.ai_skill_use_func["KurouCard"]=function(card,use,self)

	if not use.isDummy then self:speak("kurou") end

	use.card=card
end

local jijiang_skill={}
jijiang_skill.name="jijiang"
table.insert(sgs.ai_skills,jijiang_skill)
jijiang_skill.getTurnUseCard=function(self)
        if self.player:hasUsed("JijiangCard") or not self:slashIsAvailable() then return end
		local card_str = "@JijiangCard=."
		local slash = sgs.Card_Parse(card_str)
	    assert(slash)

        return slash
end

sgs.ai_skill_use_func["JijiangCard"]=function(card,use,self)
	self:sort(self.enemies, "defense")
		local target_count=0
                for _, enemy in ipairs(self.enemies) do
                        if ((self.player:canSlash(enemy, not no_distance)) or
                        (use.isDummy and (self.player:distanceTo(enemy)<=self.predictedRange)))
                                 and
                                self:objectiveLevel(enemy)>3 and
                                self:slashIsEffective(card, enemy) then

                                use.card=card
                                if use.to then
                                    use.to:append(enemy)
                                end
                                target_count=target_count+1
                                if self.slash_targets<=target_count then return end
                        end
               end

end

local guose_skill={}
guose_skill.name="guose"
table.insert(sgs.ai_skills,guose_skill)
guose_skill.getTurnUseCard=function(self,inclusive)
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
		local card_str = ("indulgence:guose[diamond:%s]=%d"):format(number, card_id)
		local indulgence = sgs.Card_Parse(card_str)

	    assert(indulgence)

        return indulgence

end


local rende_skill={}
rende_skill.name="rende"
table.insert(sgs.ai_skills, rende_skill)
rende_skill.getTurnUseCard=function(self)
	if self.player:usedTimes("RendeCard") < 2 or self:getOverflow() > 0 or self:getCard("Shit") then
		local card_id = self:getCardRandomly(self.player, "h")
		return sgs.Card_Parse("@RendeCard=" .. card_id)
	end
end

sgs.ai_skill_use_func["RendeCard"] = function(card, use, self)
    if self.player:usedTimes("RendeCard") < 2 then
		local cards = self.player:getHandcards()
		for _, friend in ipairs(self.friends_noself) do
			if friend:getHp() == 1 then
				for _, hcard in sgs.qlist(cards) do
					if hcard:inherits("Analeptic") or hcard:inherits("Peach") then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			end
			if friend:hasSkill("paoxiao") then
				for _, hcard in sgs.qlist(cards) do
					if hcard:inherits("Slash") then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			elseif friend:hasSkill("qingnang") and friend:getHp() < 2 and friend:getHandcardNum() < 1 then
				for _, hcard in sgs.qlist(cards) do
					if hcard:isRed() and not (hcard:inherits("ExNihilo") or hcard:inherits("Peach")) then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			elseif friend:hasSkill("jizhi") then
				for _, hcard in sgs.qlist(cards) do
					if hcard:getTypeId() == sgs.Card_Trick then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			elseif friend:hasSkill("guose") then
				for _, hcard in sgs.qlist(cards) do
					if hcard:getSuit() == sgs.Card_Diamond then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			elseif friend:hasSkill("leiji") then
				for _, hcard in sgs.qlist(cards) do
					if hcard:getSuit() == sgs.Card_Spade then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			elseif friend:hasSkill("xiaoji") then
				for _, hcard in sgs.qlist(cards) do
					if hcard:inherits("EquipCard") then
						use.card = sgs.Card_Parse("@RendeCard=" .. hcard:getId())
						if use.to then use.to:append(friend) end
						return
					end
				end
			end

		end
	end

	local shit
	shit = self:getCard("Shit")
	if shit then
		use.card = sgs.Card_Parse("@RendeCard=" .. shit:getId())
		self:sort(self.enemies,"hp")
		if use.to then use.to:append(self.enemies[1]) end
		return
	end

	if self:getOverflow()>0 or (self.player:isWounded() and self.player:usedTimes("RendeCard") < 2 and not self.player:isKongcheng()) then
		if #self.friends_noself == 0 then return end

		self:sort(self.friends_noself, "handcard")
		local friend = self.friends_noself[1]
		local card_id = self:getCardRandomly(self.player, "h")
		if not sgs.Sanguosha:getCard(card_id):inherits("Shit") then
			use.card = sgs.Card_Parse("@RendeCard=" .. card_id)
		end
		if use.to then use.to:append(friend) end
		return
    end

end

local zhiheng_skill={}
zhiheng_skill.name="zhiheng"
table.insert(sgs.ai_skills, zhiheng_skill)
zhiheng_skill.getTurnUseCard=function(self)
	if not self.player:hasUsed("ZhihengCard") then
		return sgs.Card_Parse("@ZhihengCard=.")
	end
end

sgs.ai_skill_use_func["ZhihengCard"] = function(card, use, self)
	local unpreferedCards={}
	local cards=sgs.QList2Table(self.player:getHandcards())

	if self.player:getHp() < 3 then
		local zcards = self.player:getCards("he")
		for _, zcard in sgs.qlist(zcards) do
			if not zcard:inherits("Peach") and not zcard:inherits("ExNihilo") then
				table.insert(unpreferedCards,zcard:getId())
			end
		end
	end

	if #unpreferedCards == 0 then
		if self:getCardsNum("Slash")>1 then
			self:sortByKeepValue(cards)
			for _,card in ipairs(cards) do
				if card:inherits("Slash") then table.insert(unpreferedCards,card:getId()) end
			end
			table.remove(unpreferedCards,1)
		end

		local num=self:getCardsNum("Jink")-1
		if self.player:getArmor() then num=num+1 end
		if num>0 then
			for _,card in ipairs(cards) do
				if card:inherits("Jink") and num>0 then
					table.insert(unpreferedCards,card:getId())
					num=num-1
				end
			end
		end
        for _,card in ipairs(cards) do
            if card:inherits("EquipCard") then
                if card:inherits("Weapon") or
                (card:inherits("DefensiveHorse") and self.player:getDefensiveHorse()) or
                card:inherits("OffensiveHorse") or
                (card:inherits("Armor") and self.player:getArmor()) or
                 card:inherits("AmazingGrace") or
                 card:inherits("Lightning") then
                    table.insert(unpreferedCards,card:getId())
                end
            end
        end

		if self.player:getWeapon() then
			table.insert(unpreferedCards, self.player:getWeapon():getId())
		end

		if self:isEquip("SilverLion") and self.player:isWounded() then
			table.insert(unpreferedCards, self.player:getArmor():getId())
		end

		local equips=self.player:getEquips()
		for _,equip in sgs.qlist(equips) do
			if equip:inherits("OffensiveHorse") and self.player:getWeapon() then
				table.insert(unpreferedCards, equip:getId())
				break
			end
		end
	end

	if #unpreferedCards>0 then
		use.card = sgs.Card_Parse("@ZhihengCard="..table.concat(unpreferedCards,"+"))
		return
	end
end

sgs.ai_skill_invoke["luoyi"]=function(self,data)
    local cards=self.player:getHandcards()
    cards=sgs.QList2Table(cards)

    for _,card in ipairs(cards) do
        if card:inherits("Slash") then

            for _,enemy in ipairs(self.enemies) do
                if self.player:canSlash(enemy, true) and
                self:slashIsEffective(card, enemy) and
                ( (not enemy:getArmor()) or (enemy:getArmor():objectName()=="renwang_shield") or (enemy:getArmor():objectName()=="vine") ) and
                enemy:getHandcardNum()< 2 then
                    if not self.player:containsTrick("indulgence") then
						self:speak("luoyi")
                        return true
                    end
                end
            end
        end
    end
    return false
end
