#include "maneuvering.h"
#include "client.h"
#include "engine.h"
#include "carditem.h"

Analeptic::Analeptic(Card::Suit suit, int number)
    :BasicCard(suit, number)
{
    setObjectName("analeptic");
    target_fixed = true;
    once = true;
}

QString Analeptic::getSubtype() const{
    return "buff_card";
}

QString Analeptic::getEffectPath(bool ) const{
    return Card::getEffectPath();
}

bool Analeptic::IsAvailable(const Player *player){
    return !player->hasUsed("Analeptic") && !player->hasUsed("WeidaiCard");
}

bool Analeptic::isAvailable(const Player *player) const{
    return IsAvailable(player);
}

void Analeptic::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);
    if(targets.isEmpty())
        room->cardEffect(this, source, source);
    else
        foreach(ServerPlayer *tmp, targets)
            room->cardEffect(this, source, tmp);
}

void Analeptic::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    // do animation
    QString who = effect.to->objectName();
    QString animation_str = QString("analeptic:%1:%2").arg(who).arg(who);
    room->broadcastInvoke("animate", animation_str);

    if(effect.to->hasFlag("dying")){
        // recover hp
        RecoverStruct recover;
        recover.card = this;
        recover.who = effect.from;
        room->recover(effect.to, recover);
    }else{
        LogMessage log;
        log.type = "#Drank";
        log.from = effect.to;
        room->sendLog(log);

        room->setPlayerFlag(effect.to, "drank");
    }
}

class GudingBladeSkill: public WeaponSkill{
public:
    GudingBladeSkill():WeaponSkill("guding_blade"){
        events << Predamage;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.card && damage.card->inherits("Slash") &&
            damage.to->isKongcheng())
        {
            Room *room = damage.to->getRoom();

            LogMessage log;
            log.type = "#GudingBladeEffect";
            log.from = player;
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

GudingBlade::GudingBlade(Suit suit, int number):Weapon(suit, number, 2){
    setObjectName("guding_blade");
    skill = new GudingBladeSkill;
}

class SilverLionSkill: public ArmorSkill{
public:
    SilverLionSkill():ArmorSkill("silver_lion"){
        events << Predamaged;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.damage > 1){
            LogMessage log;
            log.type = "#SilverLion";
            log.from = player;
            log.arg = QString::number(damage.damage);
            player->getRoom()->sendLog(log);

            damage.damage = 1;
            data = QVariant::fromValue(damage);
        }

        return false;
    }
};

SilverLion::SilverLion(Suit suit, int number):Armor(suit, number){
    setObjectName("silver_lion");
    skill = new SilverLionSkill;
}

void SilverLion::onUninstall(ServerPlayer *player) const{
    if(player->isAlive() && player->getMark("qinggang") == 0){
        RecoverStruct recover;
        recover.card = this;
        player->getRoom()->recover(player, recover);
    }
}

FireAttack::FireAttack(Card::Suit suit, int number)
    :SingleTargetTrick(suit, number, true)
{
    setObjectName("fire_attack");
}

bool FireAttack::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->isKongcheng())
        return false;

    if(to_select == Self)
        return Self->getHandcardNum() >= 2;
    else
        return true;
}

void FireAttack::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    if(effect.to->isKongcheng())
        return;

    const Card *card = room->askForCardShow(effect.to, effect.from, objectName());
    room->showCard(effect.to, card->getEffectiveId());

    QString suit_str = card->getSuitString();
    QString pattern = QString(".%1").arg(suit_str.at(0).toUpper());
    QString prompt = QString("@fire-attack:%1::%2").arg(effect.to->getGeneralName()).arg(suit_str);
    if(room->askForCard(effect.from, pattern, prompt)){
        DamageStruct damage;
        damage.card = this;
        damage.from = effect.from;
        damage.to = effect.to;
        damage.nature = DamageStruct::Fire;

        room->damage(damage);
    }

    if(card->isVirtualCard())
        delete card;
}

IronChain::IronChain(Card::Suit suit, int number)
    :TrickCard(suit, number, false)
{
    setObjectName("iron_chain");
}

QString IronChain::getSubtype() const{
    return "damage_spread";
}

QString IronChain::getEffectPath(bool is_male) const{
    return QString();
}

bool IronChain::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() >= 2)
        return false;

    return true;
}

bool IronChain::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    if(getSkillName() == "guhuo")
        return targets.length() == 1 || targets.length() == 2;
    else
        return targets.length() <= 2;
}

void IronChain::onUse(Room *room, const CardUseStruct &card_use) const{
    if(card_use.to.isEmpty()){
        room->throwCard(this);
        room->playCardEffect("@recast", card_use.from->getGeneral()->isMale());
        card_use.from->drawCards(1);
    }else
        TrickCard::onUse(room, card_use);
}

void IronChain::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    room->playCardEffect("@tiesuo", source->getGeneral()->isMale());
    TrickCard::use(room, source, targets);
}

void IronChain::onEffect(const CardEffectStruct &effect) const{
    bool chained = ! effect.to->isChained();
    effect.to->setChained(chained);

    effect.to->getRoom()->broadcastProperty(effect.to, "chained");
}

Qianliyan::Qianliyan(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("qianliyan");
}

bool Qianliyan::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select == Self)
        return false;

    if(to_select->isKongcheng())
        return false;

    return true;
}

void Qianliyan::onEffect(const CardEffectStruct &effect) const{
    effect.from->getRoom()->showAllCards(effect.to, effect.from);
}

ManeuveringPackage::ManeuveringPackage()
    :Package("maneuvering")
{
    QList<Card *> cards;

    cards << new Qianliyan(Card::Club, 12)
            << new Qianliyan(Card::Diamond, 13);

    foreach(Card *card, cards)
        card->setParent(this);

    type = CardPack;
}

ADD_PACKAGE(Maneuvering)
