#include "standard.h"
#include "standard-skillcards.h"
#include "room.h"
#include "clientplayer.h"
#include "engine.h"
#include "client.h"
#include "settings.h"

ZhihengCard::ZhihengCard(){
    target_fixed = true;
    once = true;
}

void ZhihengCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    room->throwCard(this);
    if(source->isAlive())
        room->drawCards(source, subcards.length());
}


RendeCard::RendeCard(){
    will_throw = false;
}

void RendeCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = NULL;
    if(targets.isEmpty()){
        foreach(ServerPlayer *player, room->getAlivePlayers()){
            if(player != source){
                target = player;
                break;
            }
        }
    }else
        target = targets.first();

    room->moveCardTo(this, target, Player::Hand, false);

    int old_value = source->getMark("rende");
    int new_value = old_value + subcards.length();
    room->setPlayerMark(source, "rende", new_value);

    if(old_value < 2 && new_value >= 2){
        RecoverStruct recover;
        recover.card = this;
        recover.who = source;
        room->recover(source, recover);
    }
}

JieyinCard::JieyinCard(){
    once = true;
    mute = true;
}

bool JieyinCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    return to_select->getGeneral()->isMale() && to_select->isWounded();
}

void JieyinCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();

    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;

    room->recover(effect.from, recover, true);
    room->recover(effect.to, recover, true);

    int index = -1;
    if(effect.from->getGeneral()->isMale()){
        if(effect.from == effect.to)
            index = 5;
        else if(effect.from->getHp() >= effect.to->getHp())
            index = 3;
        else
            index = 4;
    }else{
        index = 1 + qrand() % 2;
    }

    room->playSkillEffect("jieyin", index);
}

TuxiCard::TuxiCard(){
}

bool TuxiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() >= 2)
        return false;

    if(to_select == Self)
        return false;

    return !to_select->isKongcheng();
}

void TuxiCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "h", "tuxi");
    const Card *card = Sanguosha->getCard(card_id);
    room->moveCardTo(card, effect.from, Player::Hand, false);

    room->setEmotion(effect.to, "bad");
    room->setEmotion(effect.from, "good");
}

void TuxiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
   room->playSkillEffect(objectName());
   foreach(ServerPlayer *target, targets){
       CardEffectStruct effect;
       effect.card = this;
       effect.from = source;
       effect.to = target;
       effect.multiple = true;

       room->cardEffect(effect);
   }
   if(targets.length() < 2)
       source->drawCards(2-targets.length());
}

FanjianCard::FanjianCard(){
    once = true;
}

bool FanjianCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->getGeneral()->isMale() && to_select != Self;
}

void FanjianCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *zhouyu = effect.from;
    ServerPlayer *target = effect.to;
    Room *room = zhouyu->getRoom();

    int card_id = zhouyu->getRandomHandCardId();
    const Card *card = Sanguosha->getCard(card_id);
    Card::Suit suit1 = room->askForSuit(target);
    Card::Suit suit2 = room->askForSuit(target);

    LogMessage log;
    log.type = "#QiqingSuit";
    log.from = target;
    log.arg = Card::Suit2String(suit1);
    log.arg2 = Card::Suit2String(suit2);
    room->sendLog(log);

    room->showCard(zhouyu, card_id);
    room->getThread()->delay();

    if(card->getSuit() != suit1 && card->getSuit() != suit2){
        DamageStruct damage;
        damage.card = NULL;
        damage.from = zhouyu;
        damage.to = target;

        room->damage(damage);

        RecoverStruct recover;
        recover.card = NULL;
        recover.who = zhouyu;
        zhouyu->getRoom()->recover(zhouyu, recover);
    }

    if(target->isAlive()){
        target->obtainCard(card);
    }
}

KurouCard::KurouCard(){
    target_fixed = true;
}

void KurouCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    room->loseHp(source);
    if(source->isAlive())
        room->drawCards(source, 2);
}

LijianCard::LijianCard(){
    once = true;
}

bool LijianCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.isEmpty() && to_select->hasSkill("kongcheng") && to_select->isKongcheng()){
        return false;
    }

    return true;
}

bool LijianCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2;
}

void LijianCard::use(Room *room, ServerPlayer *, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *to = targets.at(0);
    ServerPlayer *from = targets.at(1);

    Duel *duel = new Duel(Card::NoSuit, 0);
    duel->setSkillName("lijian");
    duel->setCancelable(false);

    CardUseStruct use;
    use.from = from;
    use.to << to;
    use.card = duel;
    room->useCard(use);
}

QingnangCard::QingnangCard(){
    once = true;
}

bool QingnangCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->isWounded();
}

bool QingnangCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.value(0, Self)->isWounded();
}

void QingnangCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void QingnangCard::onEffect(const CardEffectStruct &effect) const{
    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;
    effect.to->getRoom()->recover(effect.to, recover);
}

GuicaiCard::GuicaiCard(){
    target_fixed = true;
    will_throw = false;
}

void GuicaiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{

}

LiuliCard::LiuliCard()
{
}


bool LiuliCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->hasFlag("slash_source"))
        return false;

    if(!Self->canSlash(to_select))
        return false;

    int card_id = subcards.first();
    if(Self->getWeapon() && Self->getWeapon()->getId() == card_id)
        return Self->distanceTo(to_select) <= 1;
    else
        return true;
}

void LiuliCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->getRoom()->setPlayerFlag(effect.to, "liuli_target");
}

JijiangCard::JijiangCard(){

}

bool JijiangCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->canSlash(to_select);
}

void JijiangCard::use(Room *room, ServerPlayer *liubei, const QList<ServerPlayer *> &targets) const{
    QList<ServerPlayer *> lieges = room->getLieges("shu", liubei);
    const Card *slash = NULL;

    QVariant tohelp = QVariant::fromValue((PlayerStar)liubei);
    foreach(ServerPlayer *liege, lieges){
        slash = room->askForCard(liege, "slash", "@jijiang-slash:" + liubei->objectName(), tohelp);
        if(slash){
            CardUseStruct card_use;
            card_use.card = slash;
            card_use.from = liubei;
            card_use.to << targets.first();

            room->useCard(card_use);
            return;
        }
    }
}

HuanzhuangCard::HuanzhuangCard(){
    target_fixed = true;
}

void HuanzhuangCard::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *diaochan = card_use.from;

    if(diaochan->getGeneralName() == "diaochan"){
        room->transfigure(diaochan, "sp_diaochan", false, false);
    }else if(diaochan->getGeneralName() == "sp_diaochan"){
        room->transfigure(diaochan, "diaochan", false, false);
    }
}

CheatCard::CheatCard(){
    target_fixed = true;
    will_throw = false;
}

void CheatCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    if(Config.FreeChoose)
        room->obtainCard(source, subcards.first());
}

