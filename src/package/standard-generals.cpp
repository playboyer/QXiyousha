#include "general.h"
#include "standard.h"
#include "skill.h"
#include "engine.h"
#include "client.h"
#include "carditem.h"
#include "serverplayer.h"
#include "room.h"
#include "standard-skillcards.h"
#include "ai.h"

class Jianxiong:public MasochismSkill{
public:
    Jianxiong():MasochismSkill("jianxiong"){
    }

    virtual void onDamaged(ServerPlayer *caocao, const DamageStruct &damage) const{
        Room *room = caocao->getRoom();
        const Card *card = damage.card;
        if(!room->obtainable(card, caocao))
            return;

        QVariant data = QVariant::fromValue(card);
        if(room->askForSkillInvoke(caocao, "jianxiong", data)){
            room->playSkillEffect(objectName());
            caocao->obtainCard(card);
        }
    }
};

class Xiuli: public TriggerSkill{
public:
    Xiuli():TriggerSkill("#xiuli"){
        events << Damaged;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();

        Room *room = player->getRoom();
        ServerPlayer *zhenyuanzi = room->findPlayerBySkillName(objectName());
        if(zhenyuanzi && damage.from == zhenyuanzi && damage.to->getEquips().length() > 0 && zhenyuanzi->askForSkillInvoke(objectName(), data)){
            int card_id = room->askForCardChosen(zhenyuanzi, damage.to , "e", "xiuli");
            const Card *card = Sanguosha->getCard(card_id);
            room->playSkillEffect("jianxiong");

            LogMessage log;
            log.type = "$Xiuli";
            log.from = zhenyuanzi;
            log.to << damage.to;
            log.card_str = QString::number(card_id);
            room->sendLog(log);

            zhenyuanzi->obtainCard(card);
        }
            return false;
        }
    };

class Hujia:public TriggerSkill{
public:
    Hujia():TriggerSkill("hujia$"){
        events << CardAsked;
        default_choice = "ignore";
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasLordSkill("hujia");
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *caocao, QVariant &data) const{
        QString pattern = data.toString();
        if(pattern != "jink")
            return false;

        Room *room = caocao->getRoom();
        QList<ServerPlayer *> lieges = room->getLieges("wei", caocao);
        if(lieges.isEmpty())
            return false;

        if(!room->askForSkillInvoke(caocao, objectName()))
            return false;

        room->playSkillEffect(objectName());
        QVariant tohelp = QVariant::fromValue((PlayerStar)caocao);
        foreach(ServerPlayer *liege, lieges){
            const Card *jink = room->askForCard(liege, "jink", "@hujia-jink:" + caocao->objectName(), tohelp);
            if(jink){
                room->provide(jink);
                return true;
            }
        }

        return false;
    }
};

class TuxiViewAsSkill: public ZeroCardViewAsSkill{
public:
    TuxiViewAsSkill():ZeroCardViewAsSkill("tuxi"){
    }

    virtual const Card *viewAs() const{
        return new TuxiCard;
    }

protected:
    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@@tuxi";
    }
};

class Tuxi:public PhaseChangeSkill{
public:
    Tuxi():PhaseChangeSkill("tuxi"){
        view_as_skill = new TuxiViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *zhangliao) const{
        if(zhangliao->getPhase() == Player::Draw){
            Room *room = zhangliao->getRoom();
            bool can_invoke = false;
            QList<ServerPlayer *> other_players = room->getOtherPlayers(zhangliao);
            foreach(ServerPlayer *player, other_players){
                if(!player->isKongcheng()){
                    can_invoke = true;
                    break;
                }
            }

            if(can_invoke && room->askForUseCard(zhangliao, "@@tuxi", "@tuxi-card"))
                return true;
        }

        return false;
    }
};

class Tiandu:public TriggerSkill{
public:
    Tiandu():TriggerSkill("tiandu"){
        frequency = Frequent;

        events << FinishJudge;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *guojia, QVariant &data) const{
        JudgeStar judge = data.value<JudgeStar>();
        CardStar card = judge->card;

        QVariant data_card = QVariant::fromValue(card);
        Room *room = guojia->getRoom();
        if(guojia->askForSkillInvoke(objectName(), data_card)){
            if(card->objectName() == "shit"){
                QString result = room->askForChoice(guojia, objectName(), "yes+no");
                if(result == "no")
                    return false;
            }

            guojia->obtainCard(judge->card);
            room->playSkillEffect(objectName());

            return true;
        }

        return false;
    }
};

class Yiji:public MasochismSkill{
public:
    Yiji():MasochismSkill("yiji"){
        frequency = Frequent;
    }

    virtual void onDamaged(ServerPlayer *guojia, const DamageStruct &damage) const{
        Room *room = guojia->getRoom();

        if(!room->askForSkillInvoke(guojia, objectName()))
            return;

        room->playSkillEffect(objectName());

        int x = damage.damage, i;
        for(i=0; i<x; i++){
            guojia->drawCards(2);
            QList<int> yiji_cards = guojia->handCards().mid(guojia->getHandcardNum() - 2);

            while(room->askForYiji(guojia, yiji_cards))
                ; // empty loop
        }

    }
};

class RedPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return ! player->hasEquip(card) && card->isRed();
    }
};

class Jiuku: public TriggerSkill{
public:
    Jiuku():TriggerSkill("jiuku"){
        events <<  Predamaged;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.card == NULL || !damage.card->inherits("Slash") || damage.to->isDead())
            return false;

        Room *room = player->getRoom();
        ServerPlayer *guanyin = room->findPlayerBySkillName(objectName());
        if(guanyin && !guanyin->isKongcheng() && damage.from != guanyin && damage.to != guanyin && guanyin->askForSkillInvoke(objectName(), data)){
            const Card *card = room->askForCard(guanyin, ".red", "@jiuku-ask:");
        if(card){
            room->playSkillEffect(objectName());
            room->throwCard(card);

            if(damage.damage > 0){
                LogMessage log;
                log.type = "#Jiuku";
                log.from = player;
                log.arg = QString::number(damage.damage);
                player->getRoom()->sendLog(log);

                damage.damage = 0;
                data = QVariant::fromValue(damage);
            }
        }
        }
            return false;
        }
    };

class Ganglie:public MasochismSkill{
public:
    Ganglie():MasochismSkill("ganglie"){

    }

    virtual void onDamaged(ServerPlayer *xiahou, const DamageStruct &damage) const{
        ServerPlayer *from = damage.from;
        Room *room = xiahou->getRoom();
        QVariant source = QVariant::fromValue(from);

        if(from && from->isAlive() && room->askForSkillInvoke(xiahou, "ganglie", source)){
            room->playSkillEffect(objectName());

            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(heart):(.*)");
            judge.good = false;
            judge.reason = objectName();
            judge.who = xiahou;

            room->judge(judge);
            if(judge.isGood()){
                if(!room->askForDiscard(from, objectName(), 2, true)){
                    DamageStruct damage;
                    damage.from = xiahou;
                    damage.to = from;

                    room->setEmotion(xiahou, "good");
                    room->damage(damage);
                }
            }else
                room->setEmotion(xiahou, "bad");
        }
    }
};

class Fankui:public MasochismSkill{
public:
    Fankui():MasochismSkill("fankui"){

    }

    virtual void onDamaged(ServerPlayer *simayi, const DamageStruct &damage) const{
        ServerPlayer *from = damage.from;
        Room *room = simayi->getRoom();
        QVariant data = QVariant::fromValue(from);
        if(from && !from->isNude() && room->askForSkillInvoke(simayi, "fankui", data)){
            int card_id = room->askForCardChosen(simayi, from, "he", "fankui");
            if(room->getCardPlace(card_id) == Player::Hand)
                room->moveCardTo(Sanguosha->getCard(card_id), simayi, Player::Hand, false);
            else
                room->obtainCard(simayi, card_id);
            room->playSkillEffect(objectName());
        }
    }
};

class GuicaiViewAsSkill:public OneCardViewAsSkill{
public:
    GuicaiViewAsSkill():OneCardViewAsSkill(""){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@guicai";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new GuicaiCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Guicai: public TriggerSkill{
public:
    Guicai():TriggerSkill("guicai"){
        view_as_skill = new GuicaiViewAsSkill;

        events << AskForRetrial;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target) && !target->isKongcheng();
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        JudgeStar judge = data.value<JudgeStar>();

        QStringList prompt_list;
        prompt_list << "@guicai-card" << judge->who->objectName()
                << "" << judge->reason << judge->card->getEffectIdString();
        QString prompt = prompt_list.join(":");

        player->tag["Judge"] = data;
        const Card *card = room->askForCard(player, "@guicai", prompt, data);

        if(card){
            // the only difference for Guicai & Guidao
            room->throwCard(judge->card);

            judge->card = Sanguosha->getCard(card->getEffectiveId());
            room->moveCardTo(judge->card, NULL, Player::Special);

            LogMessage log;
            log.type = "$ChangedJudge";
            log.from = player;
            log.to << judge->who;
            log.card_str = card->getEffectIdString();
            room->sendLog(log);

            room->sendJudgeResult(judge);
        }

        return false;
    }
};

class LuoyiBuff: public TriggerSkill{
public:
    LuoyiBuff():TriggerSkill("#luoyi"){
        events << Predamage;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasFlag("luoyi") && target->isAlive();
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *xuchu, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();

        const Card *reason = damage.card;
        if(reason == NULL)
            return false;

        if(reason->inherits("Slash") || reason->inherits("Duel")){
            LogMessage log;
            log.type = "#LuoyiBuff";
            log.from = xuchu;
            log.to << damage.to;
            log.arg = QString::number(damage.damage);
            log.arg2 = QString::number(damage.damage + 1);
            xuchu->getRoom()->sendLog(log);

            damage.damage ++;
            data = QVariant::fromValue(damage);
        }

        return false;
    }
};

class Luoyi: public DrawCardsSkill{
public:
    Luoyi():DrawCardsSkill("luoyi"){

    }

    virtual int getDrawNum(ServerPlayer *xuchu, int n) const{
        Room *room = xuchu->getRoom();
        if(room->askForSkillInvoke(xuchu, objectName())){
            room->playSkillEffect(objectName());

            xuchu->setFlags(objectName());
            return n - 1;
        }else
            return n;
    }
};

class Luoshen:public TriggerSkill{
public:
    Luoshen():TriggerSkill("luoshen"){
        events << PhaseChange << FinishJudge;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *zhenji, QVariant &data) const{
        if(event == PhaseChange && zhenji->getPhase() == Player::Start){
            Room *room = zhenji->getRoom();
            while(zhenji->askForSkillInvoke("luoshen")){
                room->playSkillEffect(objectName());

                JudgeStruct judge;
                judge.pattern = QRegExp("(.*):(spade|club):(.*)");
                judge.good = true;
                judge.reason = objectName();
                judge.who = zhenji;

                room->judge(judge);
                if(judge.isBad())
                    break;
            }

        }else if(event == FinishJudge){
            JudgeStar judge = data.value<JudgeStar>();
            if(judge->reason == objectName()){
                if(judge->card->isBlack()){
                    zhenji->obtainCard(judge->card);
                    return true;
                }
            }
        }

        return false;
    }
};

class Qingguo:public OneCardViewAsSkill{
public:
    Qingguo():OneCardViewAsSkill("qingguo"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isBlack() && !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Jink *jink = new Jink(card->getSuit(), card->getNumber());
        jink->setSkillName(objectName());
        jink->addSubcard(card->getId());
        return jink;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "jink";
    }
};

class RendeViewAsSkill:public ViewAsSkill{
public:
    RendeViewAsSkill():ViewAsSkill("rende"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(ServerInfo.GameMode == "04_1v3"
           && selected.length() + Self->getMark("rende") >= 2)
           return false;
        else
            return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;

        RendeCard *rende_card = new RendeCard;
        rende_card->addSubcards(cards);
        return rende_card;
    }
};

class Rende: public PhaseChangeSkill{
public:
    Rende():PhaseChangeSkill("rende"){
        view_as_skill = new RendeViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getPhase() == Player::NotActive
                && target->hasUsed("RendeCard");
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        target->getRoom()->setPlayerMark(target, "rende", 0);

        return false;
    }
};

class JijiangViewAsSkill:public ZeroCardViewAsSkill{
public:
    JijiangViewAsSkill():ZeroCardViewAsSkill("jijiang$"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasLordSkill("jijiang") && Slash::IsAvailable(player);
    }

    virtual const Card *viewAs() const{
        return new JijiangCard;
    }
};

class Jijiang: public TriggerSkill{
public:
    Jijiang():TriggerSkill("jijiang$"){
        events << CardAsked;
        default_choice = "ignore";

        view_as_skill = new JijiangViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasLordSkill("jijiang");
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *liubei, QVariant &data) const{
        QString pattern = data.toString();
        if(pattern != "slash")
            return false;

        Room *room = liubei->getRoom();
        QList<ServerPlayer *> lieges = room->getLieges("shu", liubei);
        if(lieges.isEmpty())
            return false;

        if(!room->askForSkillInvoke(liubei, objectName()))
            return false;

        room->playSkillEffect(objectName());

        QVariant tohelp = QVariant::fromValue((PlayerStar)liubei);
        foreach(ServerPlayer *liege, lieges){
            const Card *slash = room->askForCard(liege, "slash", "@jijiang-slash:" + liubei->objectName(), tohelp);
            if(slash){
                room->provide(slash);
                return true;
            }
        }

        return false;
    }
};

class Wusheng:public OneCardViewAsSkill{
public:
    Wusheng():OneCardViewAsSkill("wusheng"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "slash";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->isBlack();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Card *slash = new Slash(card->getSuit(), card->getNumber());
        slash->addSubcard(card->getId());
        slash->setObjectName("thunder_slash");
        slash->setSkillName(objectName());
        return slash;
    }
};

class Juanlian: public TriggerSkill{
public:
    Juanlian():TriggerSkill("#juanlian"){
        events << SlashEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target);
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(effect.slash->objectName() == "thunder_slash"){
            player->getRoom()->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Juanlian";
            log.from = player;
            log.arg = "wusheng";

            player->getRoom()->sendLog(log);

            return true;
        }

        return false;
    }
};

class Longdan:public OneCardViewAsSkill{
public:
    Longdan():OneCardViewAsSkill("longdan"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getFilteredCard();

        switch(ClientInstance->getStatus()){
        case Client::Playing:{
                // jink as slash
                return card->inherits("Jink");
            }

        case Client::Responsing:{
                QString pattern = ClientInstance->getPattern();
                if(pattern == "slash")
                    return card->inherits("Jink");
                else if(pattern == "jink")
                    return card->inherits("Slash") && card->objectName() == "thunder_slash";
            }

        default:
            return false;
        }
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "jink" || pattern == "slash";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getFilteredCard();
        if(card->inherits("Slash") && card->objectName() == "thunder_slash"){
            Jink *jink = new Jink(card->getSuit(), card->getNumber());
            jink->addSubcard(card);
            jink->setSkillName(objectName());
            return jink;
        }else if(card->inherits("Jink")){
            Slash *slash = new Slash(card->getSuit(), card->getNumber());
            slash->addSubcard(card);
            slash->setObjectName("thunder_slash");
            slash->setSkillName(objectName());
            return slash;
        }else
            return NULL;
    }
};

JingtanCard::JingtanCard(){
    once = true;
}

bool JingtanCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->getEquips().isEmpty() && to_select != Self;
}

void JingtanCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = source, *player = targets.first();

    target->obtainCard(player->getWeapon());
    target->obtainCard(player->getArmor());
    target->obtainCard(player->getDefensiveHorse());
    target->obtainCard(player->getOffensiveHorse());
    room->loseHp(target);
}

class Jingtan: public ZeroCardViewAsSkill{
public:
    Jingtan():ZeroCardViewAsSkill("jingtan"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("JingtanCard");
    }

    virtual const Card *viewAs() const{
        Card *card = new JingtanCard;
        card->setSkillName(objectName());
        return card;
    }

};

class Tieji:public SlashBuffSkill{
public:
    Tieji():SlashBuffSkill("tieji"){

    }

    virtual bool buff(const SlashEffectStruct &effect) const{
        ServerPlayer *niumowang = effect.from;

        Room *room = niumowang->getRoom();
        if(effect.from->askForSkillInvoke("tieji", QVariant::fromValue(effect))){
            room->playSkillEffect(objectName());

            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(.*):(.*)");
            judge.good = true;
            judge.who = effect.from;
            judge.reason = objectName();

            room->judge(judge);

            switch(judge.card->getSuit()){
            case Card::Heart:{
                    room->slashResult(effect, NULL);
                    return true;
                    break;
                }

            case Card::Diamond:{
                    room->slashResult(effect, NULL);
                    return true;
                    break;
                }

            case Card::Club:{
                    if(!effect.to->isNude()){
                        int to_throw = room->askForCardChosen(niumowang, effect.to, "he", objectName());
                        room->throwCard(to_throw);
                        LogMessage log;
                        log.type = "$Dismantlement";
                        log.from = effect.to;
                        log.card_str = QString::number(to_throw);
                        room->sendLog(log);
                    }

                    break;
                }

            case Card::Spade:{
                    if(!effect.to->isNude()){
                        int to_throw = room->askForCardChosen(niumowang, effect.to, "he", objectName());
                        room->throwCard(to_throw);
                        LogMessage log;
                        log.type = "$Dismantlement";
                        log.from = effect.to;
                        log.card_str = QString::number(to_throw);
                        room->sendLog(log);
                    }
                }

            default:
                break;
            }
        }

        return false;
    }
};

class Guanxing:public PhaseChangeSkill{
public:
    Guanxing():PhaseChangeSkill("guanxing"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *zhuge) const{
        if(zhuge->getPhase() == Player::Start &&
           zhuge->askForSkillInvoke(objectName()))
        {
            Room *room = zhuge->getRoom();
            room->playSkillEffect(objectName());

            int n = qMin(5, room->alivePlayerCount());
            room->doGuanxing(zhuge, room->getNCards(n, false), false);
        }

        return false;
    }
};

class Kongcheng: public ProhibitSkill{
public:
    Kongcheng():ProhibitSkill("kongcheng"){

    }

    virtual bool isProhibited(const Player *from, const Player *to, const Card *card) const{
        if(card->inherits("Slash") || card->inherits("Duel"))
            return to->isKongcheng();
        else
            return false;
    }
};

class KongchengEffect: public TriggerSkill{
public:
    KongchengEffect():TriggerSkill("#kongcheng-effect"){
        frequency = Compulsory;

        events << CardLost;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        if(player->isKongcheng()){
            CardMoveStar move = data.value<CardMoveStar>();
            if(move->from_place == Player::Hand)
                player->getRoom()->playSkillEffect("kongcheng");
        }

        return false;
    }
};

class Jizhi:public TriggerSkill{
public:
    Jizhi():TriggerSkill("jizhi"){
        frequency = Frequent;
        events << CardUsed << CardResponsed;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *yueying, QVariant &data) const{
        CardStar card = NULL;
        if(event == CardUsed){
            CardUseStruct use = data.value<CardUseStruct>();
            card = use.card;
        }else if(event == CardResponsed)
            card = data.value<CardStar>();

        if(card->isNDTrick()){
            Room *room = yueying->getRoom();
            if(room->askForSkillInvoke(yueying, objectName())){
                room->playSkillEffect(objectName());
                yueying->drawCards(1);
            }
        }

        return false;
    }
};

class Zhiheng:public ViewAsSkill{
public:
    Zhiheng():ViewAsSkill("zhiheng"){

    }

    virtual bool viewFilter(const QList<CardItem *> &, const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;

        ZhihengCard *zhiheng_card = new ZhihengCard;
        zhiheng_card->addSubcards(cards);

        return zhiheng_card;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("ZhihengCard");
    }
};

class Jiuyuan: public TriggerSkill{
public:
    Jiuyuan():TriggerSkill("jiuyuan$"){
        events << Dying << AskForPeachesDone << CardEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasLordSkill("jiuyuan");
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *sunquan, QVariant &data) const{
        Room *room =  sunquan->getRoom();
        switch(event){
        case Dying: {
                foreach(ServerPlayer *wu, room->getOtherPlayers(sunquan)){
                    if(wu->getKingdom() == "wu"){
                        room->playSkillEffect("jiuyuan", 1);
                        break;
                    }
                }
                break;
            }

        case CardEffected: {
                CardEffectStruct effect = data.value<CardEffectStruct>();
                if(effect.card->inherits("Peach") && effect.from->getKingdom() == "wu"
                   && sunquan != effect.from && sunquan->hasFlag("dying"))
                {
                    int index = effect.from->getGeneral()->isMale() ? 2 : 3;
                    room->playSkillEffect("jiuyuan", index);
                    sunquan->setFlags("jiuyuan");

                    LogMessage log;
                    log.type = "#JiuyuanExtraRecover";
                    log.from = sunquan;
                    log.to << effect.from;
                    room->sendLog(log);

                    RecoverStruct recover;
                    recover.who = effect.from;
                    room->recover(sunquan, recover);

                    room->getThread()->delay(2000);
                }

                break;
            }

        case AskForPeachesDone:{
                if(sunquan->getHp() > 0 && sunquan->hasFlag("jiuyuan"))
                    room->playSkillEffect("jiuyuan", 4);
                sunquan->setFlags("-jiuyuan");

                break;
            }

        default:
            break;
        }

        return false;
    }
};

class Yingzi:public DrawCardsSkill{
public:
    Yingzi():DrawCardsSkill("yingzi"){
        frequency = Frequent;
    }

    virtual int getDrawNum(ServerPlayer *zhouyu, int n) const{
        Room *room = zhouyu->getRoom();
        if(room->askForSkillInvoke(zhouyu, objectName())){
            room->playSkillEffect(objectName());
            return n + 1;
        }else
            return n;
    }
};

class Fanjian:public ZeroCardViewAsSkill{
public:
    Fanjian():ZeroCardViewAsSkill("fanjian"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->isKongcheng() && ! player->hasUsed("FanjianCard");
    }

    virtual const Card *viewAs() const{
        return new FanjianCard;
    }
};

class Keji: public TriggerSkill{
public:
    Keji():TriggerSkill("keji"){
        events << CardResponsed;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *lumeng, QVariant &data) const{
        CardStar card_star = data.value<CardStar>();
        if(card_star->inherits("Slash"))
            lumeng->setFlags("keji_use_slash");

        return false;
    }
};

class KejiSkip: public PhaseChangeSkill{
public:
    KejiSkip():PhaseChangeSkill("#keji-skip"){
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool onPhaseChange(ServerPlayer *lumeng) const{
        if(lumeng->getPhase() == Player::Start){
            lumeng->setFlags("-keji_use_slash");
        }else if(lumeng->getPhase() == Player::Discard){
            if(!lumeng->hasFlag("keji_use_slash") &&
               lumeng->getSlashCount() == 0 &&
               lumeng->askForSkillInvoke("keji"))
            {
                lumeng->getRoom()->playSkillEffect("keji");

                return true;
            }
        }

        return false;
    }
};

class Lianying: public TriggerSkill{
public:
    Lianying():TriggerSkill("lianying"){
        events << CardLost;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *luxun, QVariant &data) const{
        if(luxun->isKongcheng()){
            CardMoveStar move = data.value<CardMoveStar>();

            if(move->from_place == Player::Hand){
                Room *room = luxun->getRoom();
                if(room->askForSkillInvoke(luxun, objectName())){
                    room->playSkillEffect(objectName());

                    luxun->drawCards(1);
                }
            }
        }

        return false;
    }
};

class Qixi: public OneCardViewAsSkill{
public:
    Qixi():OneCardViewAsSkill("qixi"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isBlack();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Dismantlement *dismantlement = new Dismantlement(first->getSuit(), first->getNumber());
        dismantlement->addSubcard(first->getId());
        dismantlement->setSkillName(objectName());
        return dismantlement;
    }
};

class Kurou: public ZeroCardViewAsSkill{
public:
    Kurou():ZeroCardViewAsSkill("kurou"){

    }

    virtual const Card *viewAs() const{
        return new KurouCard;
    }
};

class Guose: public OneCardViewAsSkill{
public:
    Guose():OneCardViewAsSkill("guose"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Diamond;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Indulgence *indulgence = new Indulgence(first->getSuit(), first->getNumber());
        indulgence->addSubcard(first->getId());
        indulgence->setSkillName(objectName());
        return indulgence;
    }
};

class LiuliViewAsSkill: public OneCardViewAsSkill{
public:
    LiuliViewAsSkill():OneCardViewAsSkill("liuli"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@liuli";
    }

    virtual bool viewFilter(const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        LiuliCard *liuli_card = new LiuliCard;
        liuli_card->addSubcard(card_item->getFilteredCard());

        return liuli_card;
    }
};

class Liuli: public TriggerSkill{
public:
    Liuli():TriggerSkill("liuli"){
        view_as_skill = new LiuliViewAsSkill;

        events << CardEffected;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *daqiao, QVariant &data) const{
        Room *room = daqiao->getRoom();

        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(effect.card->inherits("Slash") && !daqiao->isNude() && room->alivePlayerCount() > 2){
            QList<ServerPlayer *> players = room->getOtherPlayers(daqiao);
            players.removeOne(effect.from);

            bool can_invoke = false;
            foreach(ServerPlayer *player, players){
                if(daqiao->inMyAttackRange(player)){
                    can_invoke = true;
                    break;
                }
            }

            if(can_invoke){
                QString prompt = "@liuli:" + effect.from->objectName();
                room->setPlayerFlag(effect.from, "slash_source");
                if(room->askForUseCard(daqiao, "@@liuli", prompt)){
                    foreach(ServerPlayer *player, players){
                        if(player->hasFlag("liuli_target")){
                            room->setPlayerFlag(effect.from, "-slash_source");
                            room->setPlayerFlag(player, "-liuli_target");
                            effect.to = player;

                            room->cardEffect(effect);
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }
};

class Chujia: public GameStartSkill{
public:
    Chujia():GameStartSkill("chujia"){
        frequency = Limited;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return GameStartSkill::triggerable(target) && target->getGeneralName() == "sunshangxiang";
    }

    virtual void onGameStart(ServerPlayer *player) const{
        if(player->askForSkillInvoke(objectName())){
            Room *room = player->getRoom();
            room->transfigure(player, "sp_sunshangxiang", true, false);
            room->setPlayerProperty(player, "kingdom", "shu");
        }
    }
};

class Jieyin: public ViewAsSkill{
public:
    Jieyin():ViewAsSkill("jieyin"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("JieyinCard");
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() > 2)
            return false;

        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;

        JieyinCard *jieyin_card = new JieyinCard();
        jieyin_card->addSubcards(cards);

        return jieyin_card;
    }
};

class Jinguo: public PhaseChangeSkill{
public:
    Jinguo():PhaseChangeSkill("jinguo"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *nverguo) const{
        if(nverguo->getPhase() == Player::Finish){
            Room *room = nverguo->getRoom();
            if(room->askForSkillInvoke(nverguo, objectName())){
                room->playSkillEffect(objectName());
                nverguo->drawCards(1);
            }
        }

        return false;
    }
};

class Xiaoji: public TriggerSkill{
public:
    Xiaoji():TriggerSkill("xiaoji"){
        events << CardLost;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *sunshangxiang, QVariant &data) const{
        CardMoveStar move = data.value<CardMoveStar>();
        if(move->from_place == Player::Equip){
            Room *room = sunshangxiang->getRoom();
            if(room->askForSkillInvoke(sunshangxiang, objectName())){
                room->playSkillEffect(objectName());
                sunshangxiang->drawCards(2);
            }
        }

        return false;
    }
};

class Wushuang: public TriggerSkill{
public:
    Wushuang():TriggerSkill("wushuang"){
        events << SlashProceed;

        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *lubu, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        Room *room = lubu->getRoom();
        room->playSkillEffect(objectName());

        QString slasher = lubu->objectName();

        const Card *first_jink = NULL, *second_jink = NULL;
        first_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-1:" + slasher);
        if(first_jink)
            second_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-2:" + slasher);

        Card *jink = NULL;
        if(first_jink && second_jink){
            jink = new DummyCard;
            jink->addSubcard(first_jink);
            jink->addSubcard(second_jink);
        }

        room->slashResult(effect, jink);

        return true;
    }
};

class Lijian: public OneCardViewAsSkill{
public:
    Lijian():OneCardViewAsSkill("lijian"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("LijianCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        LijianCard *lijian_card = new LijianCard;
        lijian_card->addSubcard(card_item->getCard()->getId());

        return lijian_card;
    }
};

class Jieshi:public MasochismSkill{
public:
    Jieshi():MasochismSkill("jieshi"){
        frequency = Frequent;
    }

    virtual void onDamaged(ServerPlayer *baigujing, const DamageStruct &damage) const{
        Room *room = baigujing->getRoom();

        if(!room->askForSkillInvoke(baigujing, objectName()))
            return;

        room->playSkillEffect(objectName());

        int x = damage.damage, i;
        for(i=0; i<x; i++){
            baigujing->drawCards(1);
        }

    }
};

class Qingnang: public OneCardViewAsSkill{
public:
    Qingnang():OneCardViewAsSkill("qingnang"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("QingnangCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        QingnangCard *qingnang_card = new QingnangCard;
        qingnang_card->addSubcard(card_item->getCard()->getId());

        return qingnang_card;
    }
};

class Jijiu: public OneCardViewAsSkill{
public:
    Jijiu():OneCardViewAsSkill("jijiu"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern.contains("peach") && player->getPhase() == Player::NotActive;
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isRed();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Peach *peach = new Peach(first->getSuit(), first->getNumber());
        peach->addSubcard(first->getId());
        peach->setSkillName(objectName());
        return peach;
    }
};

class Qianxun: public ProhibitSkill{
public:
    Qianxun():ProhibitSkill("qianxun"){

    }

    virtual bool isProhibited(const Player *, const Player *, const Card *card) const{
        return card->inherits("Snatch") || card->inherits("Indulgence");
    }
};

class Mashu: public DistanceSkill{
public:
    Mashu():DistanceSkill("mashu")
    {
    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        if(from->hasSkill(objectName()))
            return -1;
        else
            return 0;
    }
};

class Ruyibang: public TriggerSkill{
public:
    Ruyibang():TriggerSkill("ruyibang"){
        events << SlashEffect;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        SlashEffectStruct slash_effect = data.value<SlashEffectStruct>();
        ServerPlayer *target = slash_effect.to;

        if(target->hasFlag("triggered")){
            target->setFlags("-triggered");
            return false;
        }

        ServerPlayer *next = target->getNextAlive();

        if(player->askForSkillInvoke(objectName(), data)){
            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Ruyibang";
            log.from = player;
            log.to << target;
            log.arg = next->getGeneralName();
            room->sendLog(log);

            slash_effect.to = next;
            slash_effect.to->setFlags("triggered");
            room->setEmotion(next, "victim");
            room->slashEffect(slash_effect);
        }
        return false;
    }
};

class QishierViewAsSkill: public OneCardViewAsSkill{
public:
    QishierViewAsSkill():OneCardViewAsSkill("qishier"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getGeneral()->hasSkill("qishier");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->isRed();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        QishierCard *qishier_card = new QishierCard;
        qishier_card->addSubcard(card_item->getCard()->getId());

        return qishier_card;
    }
};

QishierCard::QishierCard(){
    once = true;
}

bool QishierCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self;
}

void QishierCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void QishierCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    QString hisname = effect.to->getGeneralName();
    QString hiskindom = effect.to->getKingdom();
    room->transfigure(effect.from, hisname, false, false);
    room->setPlayerProperty(effect.from, "kingdom", hiskindom);
    room->acquireSkill(effect.from, "qishier", false);
    effect.from->setFlags("qishier");
}

class Qishier: public PhaseChangeSkill{
public:
    Qishier():PhaseChangeSkill("qishier"){
        view_as_skill = new QishierViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        if(target->getPhase() == Player::Finish && target->hasFlag("qishier")){
            Room *room = target->getRoom();
            room->transfigure(target, parent()->objectName(), false);
            room->setPlayerProperty(target, "kingdom", "wei");
    }
        return false;
    }
};

JindouCard::JindouCard(){
    once = true;
}

bool JindouCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self;
}

void JindouCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void JindouCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *sunwukong = effect.from;
    Room *room = sunwukong->getRoom();
    sunwukong->getRoom()->swapSeat(sunwukong, effect.to);

    LogMessage log;
    log.type = "#Jindouyun";
    log.from = sunwukong;
    log.to << effect.to;
    room->sendLog(log);
}

class Jindou: public OneCardViewAsSkill{
public:
    Jindou():OneCardViewAsSkill("jindou"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("JindouCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->isBlack();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        JindouCard *jindou_card = new JindouCard;
        jindou_card->addSubcard(card_item->getCard()->getId());

        return jindou_card;
    }
};

HuluCard::HuluCard(){
    once = true;
}

bool HuluCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->isKongcheng() && to_select != Self;
}

void HuluCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = source, *player = targets.first();

    const Card *card = room->askForCardShow(target, player, objectName());
    room->showCard(target, card->getEffectiveId());
    room->throwCard(card);

    LogMessage log;
    log.type = "$Hulu";
    log.from = target;
    room->sendLog(log);

    QString suit = card->getSuitString();

    int card_id = room->askForCardChosen(target, player, "h", "hulu");
    const Card *cardr = Sanguosha->getCard(card_id);

    if(cardr->getSuitString() == suit){

        LogMessage log;
        log.type = "#Hulu";
        log.from = target;
        log.to << player;
        room->sendLog(log);

        DamageStruct damage;
        damage.card = NULL;
        damage.from = target;
        damage.to = player;

        room->damage(damage);

        bool is_public = room->getCardPlace(card_id) != Player::Hand;
        room->moveCardTo(cardr, target, Player::Hand, is_public ? true : false);
    }else{
        bool is_public = room->getCardPlace(card_id) != Player::Hand;
        room->moveCardTo(cardr, target, Player::Hand, is_public ? true : false);
    }
}

class Hulu: public ZeroCardViewAsSkill{
public:
    Hulu():ZeroCardViewAsSkill("hulu"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("HuluCard") && !player->isKongcheng();
    }

    virtual const Card *viewAs() const{
        Card *card = new HuluCard;
        card->setSkillName(objectName());
        return card;
    }

};

class Wuzhishan: public OneCardViewAsSkill{
public:
    Wuzhishan():OneCardViewAsSkill("wuzhishan"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Diamond;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Indulgence *indulgence = new Indulgence(first->getSuit(), first->getNumber());
        indulgence->addSubcard(first->getId());
        indulgence->setSkillName(objectName());
        return indulgence;
    }
};

MingceCard::MingceCard(){
    once = true;
    will_throw = false;
}

void MingceCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->obtainCard(this);

    Room *room = effect.to->getRoom();
    QString choice = room->askForChoice(effect.to, "mingce", "use+draw");
    if(choice == "use"){
        QList<ServerPlayer *> players = room->getOtherPlayers(effect.to), targets;
        foreach(ServerPlayer *player, players){
            if(effect.to->canSlash(player))
                targets << player;
        }

        if(!targets.isEmpty()){
            ServerPlayer *target = room->askForPlayerChosen(effect.from, targets, "mingce");
            room->cardEffect(new Slash(Card::NoSuit, 0), effect.to, target);
        }
    }else if(choice == "draw"){
        effect.to->drawCards(1, true);
    }
}

class Mingce: public OneCardViewAsSkill{
public:
    Mingce():OneCardViewAsSkill("mingce"){
        default_choice = "draw";
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("MingceCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *c = to_select->getCard();
        return c->getTypeId() == Card::Equip || c->inherits("Slash");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        MingceCard *card = new MingceCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

QiuyangCard::QiuyangCard(){
    once = true;
}

bool QiuyangCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->getGeneral()->isMale() && !to_select->isKongcheng() && to_select != Self;
}

void QiuyangCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = source, *player = targets.first();

    int card_id = room->askForCardChosen(target, player, "h", "qiuyang");
    const Card *cardr = Sanguosha->getCard(card_id);
    bool is_public = room->getCardPlace(card_id) != Player::Hand;
    room->moveCardTo(cardr, target, Player::Hand, is_public ? true : false);
}

class Qiuyang: public ZeroCardViewAsSkill{
public:
    Qiuyang():ZeroCardViewAsSkill("qiuyang"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("QiuyangCard");
    }

    virtual const Card *viewAs() const{
        Card *card = new QiuyangCard;
        card->setSkillName(objectName());
        return card;
    }

};

class Tuoqiao:public OneCardViewAsSkill{
public:
    Tuoqiao():OneCardViewAsSkill("tuoqiao"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Diamond;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Jink *jink = new Jink(card->getSuit(), card->getNumber());
        jink->setSkillName(objectName());
        jink->addSubcard(card->getId());
        return jink;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "jink";
    }
};

class Tianyan: public OneCardViewAsSkill{
public:
    Tianyan():OneCardViewAsSkill("tianyan"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("TianyanCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        TianyanCard *tianyan_card = new TianyanCard;
        tianyan_card->addSubcard(card_item->getCard()->getId());

        return tianyan_card;
    }
};

TianyanCard::TianyanCard(){
    once = true;
}

bool TianyanCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self;
}

void TianyanCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void TianyanCard::onEffect(const CardEffectStruct &effect) const{
    effect.from->getRoom()->showAllCards(effect.to, effect.from);
}

class Sanjiandao: public TriggerSkill{
public:
    Sanjiandao():TriggerSkill("sanjiandao"){
        events << SlashProceed;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *yangjian, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        Room *room = yangjian->getRoom();
      if(yangjian->askForSkillInvoke(objectName(), data)){
        room->playSkillEffect(objectName());

        QString slasher = yangjian->objectName();

        const Card *first_jink = NULL, *second_jink = NULL;
        first_jink = room->askForCard(effect.to, "jink", "@sanjiandao-jink-1:" + slasher);
        if(first_jink)
            second_jink = room->askForCard(effect.to, "jink", "@sanjiandao-jink-2:" + slasher);

        Card *jink = NULL;
        if(first_jink && second_jink){
            jink = new DummyCard;
            jink->addSubcard(first_jink);
            jink->addSubcard(second_jink);
        }
        room->slashResult(effect, jink);
        return true;
    }
      return false;
  }
};

class YjqishierViewAsSkill: public OneCardViewAsSkill{
public:
    YjqishierViewAsSkill():OneCardViewAsSkill("yjqishier"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getGeneral()->hasSkill("yjqishier");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->isRed();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        YjqishierCard *yjqishier_card = new YjqishierCard;
        yjqishier_card->addSubcard(card_item->getCard()->getId());

        return yjqishier_card;
    }
};

YjqishierCard::YjqishierCard(){
    once = true;
}

bool YjqishierCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self;
}

void YjqishierCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void YjqishierCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    QString hisname = effect.to->getGeneralName();
    QString hiskindom = effect.to->getKingdom();
    room->transfigure(effect.from, hisname, false, false);
    room->setPlayerProperty(effect.from, "kingdom", hiskindom);
    room->acquireSkill(effect.from, "yjqishier", false);
    effect.from->setFlags("yjqishier");
}

class Yjqishier: public PhaseChangeSkill{
public:
    Yjqishier():PhaseChangeSkill("yjqishier"){
        view_as_skill = new YjqishierViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        if(target->getPhase() == Player::Finish && target->hasFlag("yjqishier")){
            Room *room = target->getRoom();
            room->transfigure(target, parent()->objectName(), false);
            room->setPlayerProperty(target, "kingdom", "shu");

    }
        return false;
    }
};

GuanghanCard::GuanghanCard()
{
}
bool GuanghanCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{

    if(!targets.isEmpty())
        return false;

    if(to_select->hasFlag("damage_source"))
        return false;

    if(!Self->canSlash(to_select))
        return false;

    else
        return true;
}
void GuanghanCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    DamageStruct damage = effect.from->tag["GuanhanDamage"].value<DamageStruct>();
    damage.to = effect.to;
    damage.chain = true;
    room->damage(damage);
}

class GuanghanViewAsSkill: public OneCardViewAsSkill{
public:
    GuanghanViewAsSkill():OneCardViewAsSkill("guanghan"){

    }

    virtual bool isEnabledAtPlay(const Player *) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@guanghan";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->isRed();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        GuanghanCard *card = new GuanghanCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Guanghan: public TriggerSkill{
public:
    Guanghan():TriggerSkill("guanghan"){
        events << Predamaged;

        view_as_skill = new GuanghanViewAsSkill;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *change, QVariant &data) const{
        if(!change->isKongcheng()){
            DamageStruct damage = data.value<DamageStruct>();
            Room *room = change->getRoom();
            room->setPlayerFlag(damage.from, "damage_source");

            change->tag["GuanghanDamage"] = QVariant::fromValue(damage);
            if(room->askForUseCard(change, "@guanghan", "@@guanghan-card")){
                room->setPlayerFlag(damage.from, "-damage_source");
                return true;
        }
        }

        return false;
    }
};

class Jiutou:public PhaseChangeSkill{
public:
    Jiutou():PhaseChangeSkill("jiutou"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *yuansheng) const{
        if(yuansheng->getPhase() == Player::Play &&
           yuansheng->askForSkillInvoke(objectName()))
        {
            Room *room = yuansheng->getRoom();
            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "$Jiutou";
            log.from = yuansheng;
            room->sendLog(log);
        }

        return false;
    }
};

class Sanmeizhenhuo: public TriggerSkill{
public:
    Sanmeizhenhuo():TriggerSkill("sanmeizhenhuo"){
        events << Predamage;
        frequency = Frequent;
    }
    virtual bool trigger(TriggerEvent event, ServerPlayer *honghaiser, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        ServerPlayer *honghaier = damage.from;
        Room *room = damage.to->getRoom();
        if(damage.card && damage.card->inherits("Slash") && damage.card->objectName() == "fire_slash"
            && room->askForSkillInvoke(honghaiser, "sanmeizhenhuo", data)){

            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#ZhenhuoEffect";
            log.from = honghaier;
            log.to << damage.to;
            log.arg = QString::number(damage.damage);
            log.arg2 = QString::number(damage.damage + 1);
            room->sendLog(log);

            damage.damage ++;
            data = QVariant::fromValue(damage);
        }

        return false;
    }
};

class Zhenhuo: public TriggerSkill{
public:
    Zhenhuo():TriggerSkill("#zhenhuo"){
        events << SlashEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target);
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(effect.slash->objectName() == "fire_slash"){
            player->getRoom()->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Zhenhuo";
            log.from = player;
            log.arg = "sanmeizhenhuo";

            player->getRoom()->sendLog(log);

            return true;
        }

        return false;
    }
};

class Santou:public PhaseChangeSkill{
public:
    Santou():PhaseChangeSkill("santou"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *nezha) const{
        if(nezha->getPhase() == Player::Play &&
           nezha->askForSkillInvoke(objectName()))
        {
            Room *room = nezha->getRoom();
            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "$Santou";
            log.from = nezha;
            room->sendLog(log);
        }

        return false;
    }
};

class Liubi: public PhaseChangeSkill{
public:
    Liubi():PhaseChangeSkill("liubi"){
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool triggerable(const ServerPlayer *player) const{
        return player->hasSkill(objectName());
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->getPhase() == Player::Discard){
            if(player->getHandcardNum() > player->getHp())
                player->getRoom()->playSkillEffect(objectName());

            Room *room = player->getRoom();

            LogMessage log;
            log.type = "$Liubi";
            log.from = player;
            room->sendLog(log);
        }
        return false;
    }
};

XingshangCard::XingshangCard(){
}

bool XingshangCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(to_select->getJudgingArea().contains(Sanguosha->getCard(this->getSubcards().first())))
        return false;
    return targets.isEmpty();
}

void XingshangCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    effect.from->getRoom()->moveCardTo(Sanguosha->getCard(this->getSubcards().first()), effect.to, Player::Judging);
    room->broadcastInvoke("animate", QString("shengsi:%1:%2")
                          .arg(effect.from->objectName())
                          .arg(effect.to->objectName()));
}

class XingshangViewAsSkill: public OneCardViewAsSkill{
public:
    XingshangViewAsSkill():OneCardViewAsSkill("xingshang"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("Lightning");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        XingshangCard *card = new XingshangCard;
        card->addSubcard(card_item->getCard()->getId());

        return card;
    }
};

class Xingshang: public TriggerSkill{
public:
    Xingshang():TriggerSkill("xingshang"){
        view_as_skill = new XingshangViewAsSkill;
        events << Death;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        if(player->isNude())
            return false;

        Room *room = player->getRoom();
        ServerPlayer *caopi = room->findPlayerBySkillName(objectName());
        if(caopi && caopi->isAlive() && room->askForSkillInvoke(caopi, objectName(), data)){
            room->playSkillEffect(objectName());

            caopi->obtainCard(player->getWeapon());
            caopi->obtainCard(player->getArmor());
            caopi->obtainCard(player->getDefensiveHorse());
            caopi->obtainCard(player->getOffensiveHorse());

            DummyCard *all_cards = player->wholeHandCards();
            if(all_cards){
                room->moveCardTo(all_cards, caopi, Player::Hand, false);
                delete all_cards;
            }
        }

        return false;
    }
};

GuidaoCard::GuidaoCard(){
    target_fixed = true;
    will_throw = false;
}

void GuidaoCard::use(Room *room, ServerPlayer *zhangjiao, const QList<ServerPlayer *> &targets) const{

}

class GuidaoViewAsSkill:public OneCardViewAsSkill{
public:
    GuidaoViewAsSkill():OneCardViewAsSkill(""){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@guidao";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isBlack();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        GuidaoCard *card = new GuidaoCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Guidao: public TriggerSkill{
public:
    Guidao():TriggerSkill("guidao"){
        view_as_skill = new GuidaoViewAsSkill;
        events << AskForRetrial;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        if(!TriggerSkill::triggerable(target))
            return false;

        if(target->isKongcheng()){
            bool has_black = false;
            int i;
            for(i=0; i<4; i++){
                const EquipCard *equip = target->getEquip(i);
                if(equip && equip->isBlack()){
                    has_black = true;
                    break;
                }
            }

            return has_black;
        }else
            return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        JudgeStar judge = data.value<JudgeStar>();

        QStringList prompt_list;
        prompt_list << "@guidao-card" << judge->who->objectName()
                << "" << judge->reason << judge->card->getEffectIdString();
        QString prompt = prompt_list.join(":");

        player->tag["Judge"] = data;
        const Card *card = room->askForCard(player, "@guidao", prompt, data);

        if(card){
            // the only difference for Guicai & Guidao
            player->obtainCard(judge->card);

            judge->card = Sanguosha->getCard(card->getEffectiveId());
            room->moveCardTo(judge->card, NULL, Player::Special);

            LogMessage log;
            log.type = "$ChangedJudge";
            log.from = player;
            log.to << judge->who;
            log.card_str = card->getEffectIdString();
            room->sendLog(log);

            room->sendJudgeResult(judge);
        }

        return false;
    }
};

class Sihai: public TriggerSkill{
public:
    Sihai():TriggerSkill("sihai"){
        events << Damaged;
        frequency = Frequent;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.nature == DamageStruct::Normal || damage.nature == DamageStruct::Fire)
            return false;

        ServerPlayer *longwang = room->findPlayerBySkillName(objectName());
        if(longwang == NULL || !room->askForSkillInvoke(longwang, objectName()))
            return false;

        QString choice = room->askForChoice(longwang, objectName(), "black+red");
        room->playSkillEffect(objectName());

        for(int i = 0; i < 1; i++){
        int card_id = room->drawCard();

        room->moveCardTo(Sanguosha->getCard(card_id), NULL, Player::Special, true);
        room->getThread()->delay();

        const Card *card = Sanguosha->getCard(card_id);

        if((choice == "black" && card->isBlack()) ||
           (choice == "red" && card->isRed())){

            LogMessage log;
            log.type = "$TakeAG";
            log.from = longwang;
            log.card_str = QString::number(card_id);
            room->sendLog(log);

            room->obtainCard(longwang, card_id);
        }

        else{
            LogMessage log;
            log.type = "$Sihai";
            log.from = longwang;
            log.card_str = QString::number(card_id);
            room->sendLog(log);

            room->throwCard(card_id);
        }
    }
        return false;
    }
};

class Zhizhu: public TriggerSkill{
public:
    Zhizhu():TriggerSkill("#zhizhu"){
        events << SlashEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target);
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(effect.slash->objectName() == "thunder_slash"){
            player->getRoom()->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Zhizhu";
            log.from = player;
            log.arg = "sihai";

            player->getRoom()->sendLog(log);

            return true;
        }

        return false;
    }
};

class Bajiaoshan: public TriggerSkill{
public:
    Bajiaoshan():TriggerSkill("bajiaoshan"){
        events << DamageComplete;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();

        if(damage.from == NULL)
            return false;

        if(!damage.from->hasSkill(objectName()))
            return false;

        if(!damage.card || (!damage.card->inherits("Slash")))
            return false;

        ServerPlayer *tieshan = damage.from;
        ServerPlayer *next = damage.to->getNextAlive();
        Room *room = tieshan->getRoom();

        if(room->askForSkillInvoke(tieshan, "bajiaoshan", data)){

            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Baijiao";
            log.from = damage.to;
            log.to << next;
            room->sendLog(log);

            room->swapSeat(damage.to, next);
        }

        return false;
    }
};

class Luocha: public TriggerSkill{
public:
    Luocha():TriggerSkill("#luocha"){
        events << SlashEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target);
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(effect.slash->objectName() != "fire_slash" && effect.slash->objectName() != "thunder_slash"){
            player->getRoom()->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Luocha";
            log.from = player;
            log.arg = "bajiaoshan";

            player->getRoom()->sendLog(log);

            return true;
        }

        return false;
    }
};

class Dingfeng: public TriggerSkill{
public:
    Dingfeng():TriggerSkill("dingfeng"){
        events << CardEffected;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        CardEffectStruct effect = data.value<CardEffectStruct>();
        Room *room = player->getRoom();
        ServerPlayer *lingji = room->findPlayerBySkillName(objectName());
        if(lingji == NULL || !lingji->inMyAttackRange(player))
            return false;

        if(effect.from != lingji && effect.to == lingji && effect.card->inherits("TrickCard")){
            if(room->askForSkillInvoke(lingji, objectName(), data)){
                room->playSkillEffect(objectName());

                JudgeStruct judge;
                judge.pattern = QRegExp("(.*):(club|diamond):(.*)");
                judge.who = lingji;
                judge.good = true;
                room->judge(judge);

                if(judge.isGood()){
                    LogMessage log;
                    log.type = "#Dingfeng";
                    log.from = effect.to;
                    log.to << effect.from;
                    log.arg = effect.card->objectName();
                    room->sendLog(log);

                    return true;
                }
            }
        }

        return false;
    }
};

class Feilong:public PhaseChangeSkill{
public:
    Feilong():PhaseChangeSkill("feilong"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *lingji) const{
        if(lingji->getPhase() == Player::Play &&
           lingji->getWeapon() == NULL && lingji->askForSkillInvoke(objectName()))
        {
            Room *room = lingji->getRoom();
            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "$Feilong";
            log.from = lingji;
            room->sendLog(log);
        }

        return false;
    }
};

void StandardPackage::addGenerals(){

    //佛阵营
    General *tangseng = new General(this, "tangseng$", "wei", 4);
    tangseng->addSkill(new Tuxi);
    tangseng->addSkill(new Hujia);

    General *sunwukong = new General(this, "sunwukong", "wei", 4);
    sunwukong->addSkill(new Ruyibang);
    sunwukong->addSkill(new Qishier);
    sunwukong->addSkill(new Jindou);

    General *zhubajie = new General(this, "zhubajie", "wei", 4);
    zhubajie->addSkill(new Longdan);
    zhubajie->addSkill(new Jingtan);

    General *shawujing = new General(this, "shawujing", "wei", 4);
    shawujing->addSkill(new Wusheng);
    shawujing->addSkill(new Juanlian);

    related_skills.insertMulti("wusheng", "#juanlian");

    General *rulai = new General(this, "rulai", "wei", 4);
    rulai->addSkill(new Wuzhishan);
    rulai->addSkill(new Mingce);

    General *guanyin = new General(this, "guanyin", "wei", 3, false);
    guanyin->addSkill(new Yiji);
    guanyin->addSkill(new Jiuku);

    General *mile = new General(this, "mile", "wei", 4);
    mile->addSkill(new Lianying);
    mile->addSkill(new Keji);
    mile->addSkill(new KejiSkip);

    related_skills.insertMulti("keji", "#keji-skip");

    General *lingji = new General(this, "lingji", "wei", 4);
    lingji->addSkill(new Dingfeng);
    lingji->addSkill(new Feilong);

    //仙阵营
    General *yuhuang = new General(this, "yuhuang$", "shu", 4);
    yuhuang->addSkill(new Rende);
    yuhuang->addSkill(new Jijiang);

    General *wangmu = new General(this, "wangmu", "shu", 3, false);
    wangmu->addSkill(new Qingnang);
    wangmu->addSkill(new Jijiu);

    General *yangjian = new General(this, "yangjian", "shu", 4);
    yangjian->addSkill(new Sanjiandao);
    yangjian->addSkill(new Yjqishier);
    yangjian->addSkill(new Tianyan);

    General *nezha = new General(this, "nezha", "shu", 4);
    nezha->addSkill(new Santou);
    nezha->addSkill(new Liubi);

    General *puti = new General(this, "puti", "shu", 4);
    puti->addSkill(new Guanxing);
    puti->addSkill(new Guicai);

    General *longwang = new General(this, "longwang", "shu", 4);
    longwang->addSkill(new Sihai);
    longwang->addSkill(new Zhizhu);

    related_skills.insertMulti("sihai", "#zhizhu");

    General *zhenyuanzi = new General(this, "zhenyuanzi", "shu");
    zhenyuanzi->addSkill(new Jianxiong);
    zhenyuanzi->addSkill(new Xiuli);

    related_skills.insertMulti("jianxiong", "#xiuli");

    General *yanwang = new General(this, "yanwang", "shu", 4);
    yanwang->addSkill(new Guidao);
    yanwang->addSkill(new Xingshang);

    General *change = new General(this, "change", "shu", 3, false);
    change->addSkill(new Guanghan);

    //妖阵营
    General *niumowang = new General(this, "niumowang$", "wu", 4);
    niumowang->addSkill(new Tieji);

    General *tieshan = new General(this, "tieshan", "wu", 4, false);
    tieshan->addSkill(new Bajiaoshan);
    tieshan->addSkill(new Luocha);

    related_skills.insertMulti("bajiaoshan", "#luocha");

    General *yinjiao = new General(this, "yinjiao", "wu", 4);
    yinjiao->addSkill(new Hulu);

    General *baigujing = new General(this, "baigujing", "wu", 3, false);
    baigujing->addSkill(new Lijian);
    baigujing->addSkill(new Jieshi);

    General *honghaier = new General(this, "honghaier", "wu", 4);
    honghaier->addSkill(new Sanmeizhenhuo);
    honghaier->addSkill(new Zhenhuo);

    related_skills.insertMulti("sanmeizhenhuo", "#zhenhuo");

    General *baimaoshu = new General(this, "baimaoshu", "wu", 3, false);
    baimaoshu->addSkill(new Qiuyang);
    baimaoshu->addSkill(new Tuoqiao);

    General *zhizhujing = new General(this, "zhizhujing", "wu", 3, false);
    zhizhujing->addSkill(new Fanjian);

    General *yuansheng = new General(this, "yuansheng", "wu", 4);
    yuansheng->addSkill(new Jiutou);

    //人阵营
    General *guowang = new General(this, "guowang", "qun", 3, false);
    guowang->addSkill(new Jieyin);
    guowang->addSkill(new Jinguo);

    // for skill cards
    addMetaObject<ZhihengCard>();
    addMetaObject<RendeCard>();
    addMetaObject<TuxiCard>();
    addMetaObject<JieyinCard>();
    addMetaObject<KurouCard>();
    addMetaObject<LijianCard>();
    addMetaObject<FanjianCard>();
    addMetaObject<GuicaiCard>();
    addMetaObject<QingnangCard>();
    addMetaObject<LiuliCard>();
    addMetaObject<JijiangCard>();
    addMetaObject<HuanzhuangCard>();
    addMetaObject<CheatCard>();
    addMetaObject<QishierCard>();
    addMetaObject<JindouCard>();
    addMetaObject<HuluCard>();
    addMetaObject<QiuyangCard>();
    addMetaObject<TianyanCard>();
    addMetaObject<YjqishierCard>();
    addMetaObject<GuanghanCard>();
    addMetaObject<GuidaoCard>();
    addMetaObject<JingtanCard>();
    addMetaObject<MingceCard>();
    addMetaObject<XingshangCard>();

    patterns[".red"] = new RedPattern;
}
