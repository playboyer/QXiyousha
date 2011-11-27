#include "standard.h"
#include "standard-equips.h"
#include "general.h"
#include "engine.h"
#include "client.h"
#include "room.h"
#include "carditem.h"

Slash::Slash(Suit suit, int number): BasicCard(suit, number)
{
    setObjectName("slash");
    nature = DamageStruct::Normal;
}

DamageStruct::Nature Slash::getNature() const{
    return nature;
}

void Slash::setNature(DamageStruct::Nature nature){
    this->nature = nature;
}

bool Slash::IsAvailable(const Player *player){
    if(player->hasFlag("tianyi_failed") || player->hasFlag("xianzhen_failed"))
        return false;

    return player->hasWeapon("crossbow") || player->canSlashWithoutCrossbow();
}

bool Slash::isAvailable(const Player *player) const{
    return IsAvailable(player);
}

QString Slash::getSubtype() const{
    return "attack_card";
}

void Slash::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    BasicCard::use(room, source, targets);

    if(source->hasFlag("drank")){
        LogMessage log;
        log.type = "#UnsetDrank";
        log.from = source;
        room->sendLog(log);

        room->setPlayerFlag(source, "-drank");
    }
}

void Slash::onEffect(const CardEffectStruct &card_effect) const{
    Room *room = card_effect.from->getRoom();

    if(this->objectName() == "fire_slash"){
    room->broadcastInvoke("animate", QString("fire-slash:%1").arg(card_effect.from->objectName()));
    room->setEmotion(card_effect.to, "victim");
}else
    if(this->objectName() == "thunder_slash"){
    room->broadcastInvoke("animate", QString("water-slash:%1").arg(card_effect.from->objectName()));
    room->setEmotion(card_effect.to, "victim");
}else{
    room->broadcastInvoke("animate", QString("wind-slash:%1").arg(card_effect.from->objectName()));
    room->setEmotion(card_effect.to, "victim");
}
    SlashEffectStruct effect;
    effect.from = card_effect.from;
    effect.nature = nature;
    effect.slash = this;

    effect.to = card_effect.to;
    effect.drank = effect.from->hasFlag("drank");

    room->slashEffect(effect);
}

bool Slash::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return !targets.isEmpty();
}

bool Slash::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int slash_targets = 1;
    if(Self->hasWeapon("halberd") && Self->isLastHandCard(this)){
        slash_targets = 3;
    }
    if(Self->hasSkill("jiutou") && Self->isLastHandCard(this)){
        slash_targets = 9;
    }

    bool distance_limit = true;

    if(Self->hasFlag("tianyi_success")){
        distance_limit = false;
        slash_targets ++;
    }

    if(Self->hasSkill("shenji") && Self->getWeapon() == NULL)
        slash_targets = 3;

    if(targets.length() >= slash_targets)
        return false;

    if(inherits("WushenSlash")){
        distance_limit = false;
    }

    return Self->canSlash(to_select, distance_limit);
}

Jink::Jink(Suit suit, int number):BasicCard(suit, number){
    setObjectName("jink");

    target_fixed = true;
}

QString Jink::getSubtype() const{
    return "defense_card";
}

bool Jink::isAvailable(const Player *) const{
    return false;
}

Peach::Peach(Suit suit, int number):BasicCard(suit, number){
    setObjectName("peach");
    target_fixed = true;
}

QString Peach::getSubtype() const{
    return "recover_card";
}

QString Peach::getEffectPath(bool is_male) const{
    return Card::getEffectPath();
}

void Peach::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    if(targets.isEmpty())
        room->cardEffect(this, source, source);
    else
        foreach(ServerPlayer *tmp, targets)
            room->cardEffect(this, source, tmp);
}

void Peach::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    // do animation
    room->broadcastInvoke("animate", QString("peach:%1:%2")
                          .arg(effect.from->objectName())
                          .arg(effect.to->objectName()));

    // recover hp
    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;

    room->recover(effect.to, recover);
}

bool Peach::isAvailable(const Player *player) const{
    return player->isWounded();
}

QString Crossbow::getEffectPath(bool ) const{
    return "audio/card/common/zijinling.ogg";
}

Crossbow::Crossbow(Suit suit, int number)
    :Weapon(suit, number, 1)
{
    setObjectName("crossbow");
}

class DoubleSwordSkill: public WeaponSkill{
public:
    DoubleSwordSkill():WeaponSkill("double_sword"){
        events << SlashEffect;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        Room *room = player->getRoom();

        if(effect.from->getGeneral()->isMale() != effect.to->getGeneral()->isMale()){
            if(effect.from->askForSkillInvoke(objectName())){
                room->broadcastInvoke("playAudio", "zhaoyaojing");

                bool draw_card = false;

                if(effect.to->isKongcheng())
                    draw_card = true;
                else{
                    QString prompt = "double-sword-card:" + effect.from->getGeneralName();
                    const Card *card = room->askForCard(effect.to, ".", prompt);
                    if(card){
                        room->throwCard(card);
                    }else
                        draw_card = true;
                }

                if(draw_card)
                    effect.from->drawCards(1);
            }
        }

        return false;
    }
};

QString DoubleSword::getEffectPath(bool ) const{
    return "audio/card/common/zhaoyaojing.ogg";
}

DoubleSword::DoubleSword(Suit suit, int number)
    :Weapon(suit, number, 2)
{
    setObjectName("double_sword");
    skill = new DoubleSwordSkill;
}

class QinggangSwordSkill: public WeaponSkill{
public:
    QinggangSwordSkill():WeaponSkill("qinggang_sword"){
        events << SlashEffect;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        effect.to->addMark("qinggang");

        return false;
    }
};

QString QinggangSword::getEffectPath(bool ) const{
    return "audio/card/common/tiangangdao.ogg";
}

QinggangSword::QinggangSword(Suit suit, int number)
    :Weapon(suit, number, 3)
{
    setObjectName("qinggang_sword");

    skill = new QinggangSwordSkill;
}

class BladeSkill : public WeaponSkill{
public:
    BladeSkill():WeaponSkill("blade"){
        events << SlashMissed;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(effect.to->hasSkill("kongcheng") && effect.to->isKongcheng())
            return false;

        Room *room = player->getRoom();
        const Card *card = room->askForCard(player, "slash", "blade-slash");
        if(card){
            // if player is drank, unset his flag
            if(player->hasFlag("drank"))
                room->setPlayerFlag(player, "-drank");

            CardUseStruct use;
            use.card = card;
            use.from = player;
            use.to << effect.to;

            room->broadcastInvoke("playAudio", "jiuchidingpa");
            room->useCard(use, false);
        }

        return false;
    }
};

QString Blade::getEffectPath(bool ) const{
    return "audio/card/common/jiuchidingpa.ogg";
}

Blade::Blade(Suit suit, int number)
    :Weapon(suit, number, 3)
{
    setObjectName("blade");
    skill = new BladeSkill;
}

class SpearSkill: public ViewAsSkill{
public:
    SpearSkill():ViewAsSkill("spear"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "slash";
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return selected.length() < 2 && !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;

        const Card *first = cards.at(0)->getFilteredCard();
        const Card *second = cards.at(1)->getFilteredCard();

        Card::Suit suit = Card::NoSuit;
        if(first->isBlack() && second->isBlack())
            suit = Card::Spade;
        else if(first->isRed() && second->isRed())
            suit = Card::Heart;

        Slash *slash = new Slash(suit, 0);
        slash->setSkillName(objectName());
        slash->setObjectName("fire_slash");
        slash->addSubcard(first);
        slash->addSubcard(second);

        return slash;
    }
};

QString Spear::getEffectPath(bool ) const{
    return "audio/card/common/huojianqiang.ogg";
}

Spear::Spear(Suit suit, int number)
    :Weapon(suit, number, 3)
{
    setObjectName("spear");
    attach_skill = true;
}

class AxeViewAsSkill: public ViewAsSkill{
public:
    AxeViewAsSkill():ViewAsSkill("axe"){

    }

    virtual bool isEnabledAtPlay(const Player *) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *, const QString &pattern) const{
        return pattern == "@axe";
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() >= 2)
            return false;

        if(to_select->getCard() == Self->getWeapon())
            return false;

        return true;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;

        DummyCard *card = new DummyCard;
        card->setSkillName(objectName());
        card->addSubcards(cards);
        return card;
    }
};

class AxeSkill: public WeaponSkill{
public:
    AxeSkill():WeaponSkill("axe"){
        events << SlashMissed;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        Room *room = player->getRoom();
        CardStar card = room->askForCard(player, "@axe", "@axe:" + effect.to->objectName());
        if(card){
            QList<int> card_ids = card->getSubcards();
            foreach(int card_id, card_ids){
                LogMessage log;
                log.type = "$DiscardCard";
                log.from = player;
                log.card_str = QString::number(card_id);

                room->sendLog(log);
            }

            LogMessage log;
            log.type = "#AxeSkill";
            log.from = player;
            log.to << effect.to;
            room->sendLog(log);

            room->broadcastInvoke("playAudio", "xiangyaobaochan");
            room->slashResult(effect, NULL);
        }

        return false;
    }
};

QString Axe::getEffectPath(bool ) const{
    return "audio/card/common/xiangyaobaochan.ogg";
}

Axe::Axe(Suit suit, int number)
    :Weapon(suit, number, 2)
{
    setObjectName("axe");
    skill = new AxeSkill;
    attach_skill = true;
}

QString Halberd::getEffectPath(bool ) const{
    return "audio/card/common/jingubang.ogg";
}

Halberd::Halberd(Suit suit, int number)
    :Weapon(suit, number, 4)
{
    setObjectName("halberd");
}

class KylinBowSkill: public WeaponSkill{
public:
    KylinBowSkill():WeaponSkill("kylin_bow"){
        events << SlashHit;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        QStringList weapons;
        if(effect.to->getWeapon())
            weapons << "weapon";

        if(weapons.isEmpty())
            return false;

        Room *room = player->getRoom();
        if(!player->askForSkillInvoke(objectName(), data))
            return false;

        QString weapon_type;
        if(weapons.length() == 2)
            weapon_type = room->askForChoice(player, objectName(), weapons.join("+"));
        else
            weapon_type = weapons.first();

        if(weapon_type == "weapon")
            room->broadcastInvoke("playAudio", "jingangzhuo");
            player->obtainCard(effect.to->getWeapon());

        return false;
    }
};

QString KylinBow::getEffectPath(bool ) const{
    return "audio/card/common/jingangzhuo.ogg";
}

KylinBow::KylinBow(Suit suit, int number)
    :Weapon(suit, number, 3)
{
    setObjectName("kylin_bow");
    skill = new KylinBowSkill;
}

class EightDiagramSkill: public ArmorSkill{
private:
    EightDiagramSkill():ArmorSkill("eight_diagram"){
        events << CardAsked;
    }

public:
    static EightDiagramSkill *GetInstance(){
        static EightDiagramSkill *instance = NULL;
        if(instance == NULL)
            instance = new EightDiagramSkill;

        return instance;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        QString asked = data.toString();
        if(asked == "jink"){
            Room *room = player->getRoom();
            if(room->askForSkillInvoke(player, objectName())){
                room->broadcastInvoke("playAudio", "xianyi");

                JudgeStruct judge;
                judge.pattern = QRegExp("(.*):(heart|diamond):(.*)");
                judge.good = true;
                judge.reason = objectName();
                judge.who = player;

                room->judge(judge);
                if(judge.isGood()){
                    Jink *jink = new Jink(Card::NoSuit, 0);
                    jink->setSkillName(objectName());
                    room->provide(jink);
                    if(player->getGeneral()->isMale()){
                        room->broadcastInvoke("playAudio", "male-jink");
                    }else{
                        room->broadcastInvoke("playAudio", "female-jink");
                        }
                    return true;
                }}
        }
        return false;
    }
};

QString EightDiagram::getEffectPath(bool ) const{
    return "audio/card/common/wucaixianyi.ogg";
}

EightDiagram::EightDiagram(Suit suit, int number)
    :Armor(suit, number){
    setObjectName("eight_diagram");
    skill = EightDiagramSkill::GetInstance();
}

AmazingGrace::AmazingGrace(Suit suit, int number)
    :GlobalEffect(suit, number)
{
    setObjectName("amazing_grace");
}

void AmazingGrace::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    QList<ServerPlayer *> players = targets.isEmpty() ? room->getAllPlayers() : targets;
    QList<int> card_ids = room->getNCards(players.length());
    room->fillAG(card_ids);

    QVariantList ag_list;
    foreach(int card_id, card_ids)
        ag_list << card_id;
    room->setTag("AmazingGrace", ag_list);

    GlobalEffect::use(room, source, players);

    ag_list = room->getTag("AmazingGrace").toList();

    // throw the rest cards
    foreach(QVariant card_id, ag_list){
        room->takeAG(NULL, card_id.toInt());
    }

    room->broadcastInvoke("clearAG");
}

void AmazingGrace::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    QVariantList ag_list = room->getTag("AmazingGrace").toList();
    QList<int> card_ids;
    foreach(QVariant card_id, ag_list)
        card_ids << card_id.toInt();

    int card_id = room->askForAG(effect.to, card_ids, false, objectName());
    card_ids.removeOne(card_id);

    room->takeAG(effect.to, card_id);
    ag_list.removeOne(card_id);

    room->setTag("AmazingGrace", ag_list);
}

GodSalvation::GodSalvation(Suit suit, int number)
    :GlobalEffect(suit, number)
{
    setObjectName("god_salvation");
}

bool GodSalvation::isCancelable(const CardEffectStruct &effect) const{
    return effect.to->isWounded();
}

void GodSalvation::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;
    room->recover(effect.to, recover);
}

SavageAssault::SavageAssault(Suit suit, int number)
    :AOE(suit, number)
{
    setObjectName("savage_assault");
}

void SavageAssault::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    const Card *slash = room->askForCard(effect.to, "slash", "savage-assault-slash:" + effect.from->objectName());
    if(slash)
        if(slash->objectName() == "fire_slash"){
        room->broadcastInvoke("animate", QString("fire-slash:%1").arg(effect.to->objectName()));
    }else
        if(slash->objectName() == "thunder_slash"){
        room->broadcastInvoke("animate", QString("water-slash:%1").arg(effect.to->objectName()));
    }else{
        room->broadcastInvoke("animate", QString("wind-slash:%1").arg(effect.to->objectName()));
    }
    else{
        DamageStruct damage;
        damage.card = this;
        damage.damage = 1;
        damage.to = effect.to;
        damage.nature = DamageStruct::Normal;

        damage.from = effect.from;
        room->damage(damage);
    }
}

ArcheryAttack::ArcheryAttack(Card::Suit suit, int number)
    :AOE(suit, number)
{
    setObjectName("archery_attack");
}

void ArcheryAttack::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    const Card *jink = room->askForCard(effect.to, "jink", "archery-attack-jink:" + effect.from->objectName());
    if(jink)
        room->broadcastInvoke("animate", QString("jink:%1").arg(effect.to->objectName()));
    else{
        DamageStruct damage;
        damage.card = this;
        damage.damage = 1;
        damage.from = effect.from;
        damage.to = effect.to;
        damage.nature = DamageStruct::Normal;

        room->damage(damage);
    }
}

void SingleTargetTrick::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    if(!targets.isEmpty()){
        foreach(ServerPlayer *tmp, targets){
            effect.to = tmp;
            room->cardEffect(effect);
        }
    }
    else{
        effect.to = source;
        room->cardEffect(effect);
    }
}

Collateral::Collateral(Card::Suit suit, int number)
    :SingleTargetTrick(suit, number, false)
{
    setObjectName("collateral");
}

bool Collateral::isAvailable(const Player *player) const{
    foreach(const Player *p, player->getSiblings()){
        if(p->getWeapon() && p->isAlive())
            return true;
    }

    return false;
}

bool Collateral::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2;
}

bool Collateral::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.isEmpty()){
        if(to_select->hasSkill("weimu") && isBlack())
            return false;

        return to_select->getWeapon() && to_select != Self;
    }else if(targets.length() == 1){
        const Player *first = targets.first();
        return first != Self && first->getWeapon() && first->canSlash(to_select);
    }else
        return false;
}

void Collateral::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *killer = targets.at(0);
    QList<ServerPlayer *> victims = targets;
    if(victims.length() > 1)
        victims.removeAt(0);
    const Weapon *weapon = killer->getWeapon();

    if(weapon == NULL)
        return;

    bool on_effect = room->cardEffect(this, source, killer);
    if(on_effect){
        QString prompt = QString("collateral-slash:%1:%2")
                         .arg(source->objectName()).arg(victims.first()->objectName());
        const Card *slash = room->askForCard(killer, "slash", prompt);
        if(slash){
            CardUseStruct use;
            use.card = slash;
            use.from = killer;
            use.to = victims;
            room->useCard(use);
        }else{
            source->obtainCard(weapon);
        }
    }
}

Nullification::Nullification(Suit suit, int number)
    :SingleTargetTrick(suit, number, false)
{
    setObjectName("nullification");
}

void Nullification::use(Room *room, ServerPlayer *, const QList<ServerPlayer *> &) const{
    // does nothing, just throw it
    room->throwCard(this);
}

bool Nullification::isAvailable(const Player *) const{
    return false;
}

ExNihilo::ExNihilo(Suit suit, int number)
    :SingleTargetTrick(suit, number, false)
{
    setObjectName("ex_nihilo");
    target_fixed = true;
}

void ExNihilo::onEffect(const CardEffectStruct &effect) const{
    effect.to->drawCards(2);
}

Duel::Duel(Suit suit, int number)
    :SingleTargetTrick(suit, number, true)
{
    setObjectName("duel");
}

bool Duel::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(to_select->hasSkill("kongcheng") && to_select->isKongcheng())
        return false;

    if(to_select == Self)
        return false;

    return targets.isEmpty();
}

void Duel::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *first = effect.to;
    ServerPlayer *second = effect.from;
    Room *room = first->getRoom();

    room->setEmotion(first, "duel-a");
    room->setEmotion(second, "duel-b");

    forever{
        if(second->hasSkill("wushuang")){
            room->playSkillEffect("wushuang");
            const Card *slash = room->askForCard(first, "slash", "@wushuang-slash-1:" + second->objectName());
            if(slash == NULL)
                break;

            slash = room->askForCard(first, "slash", "@wushuang-slash-2:" + second->objectName());
            if(slash == NULL)
                break;
        }else
        if(second->hasSkill("sanjiandao") && second->askForSkillInvoke("sanjiandao")){
            room->playSkillEffect("sanjiandao");
            const Card *slash = room->askForCard(first, "slash", "@sanjiandao-slash-1:" + second->objectName());
            if(slash == NULL)
                break;

            slash = room->askForCard(first, "slash", "@sanjiandao-slash-2:" + second->objectName());
            if(slash == NULL)
                break;
        }else{
            const Card *slash = room->askForCard(first, "slash", "duel-slash:" + second->objectName());
            if(slash == NULL)
                break;
        }

        qSwap(first, second);
    }

    DamageStruct damage;
    damage.card = this;
    damage.from = second;
    damage.to = first;

    room->damage(damage);
}

Snatch::Snatch(Suit suit, int number):SingleTargetTrick(suit, number, true) {
    setObjectName("snatch");
}

bool Snatch::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->isAllNude())
        return false;

    if(to_select == Self)
        return false;

    if(Self->distanceTo(to_select) > 1 && !Self->hasSkill("qicai"))
        return false;

    return true;
}

void Snatch::onEffect(const CardEffectStruct &effect) const{
    if(effect.to->isAllNude())
        return;

    Room *room = effect.to->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "hej", objectName());

    if(room->getCardPlace(card_id) == Player::Hand)
        room->moveCardTo(Sanguosha->getCard(card_id), effect.from, Player::Hand, false);
    else
        room->obtainCard(effect.from, card_id);
}

Dismantlement::Dismantlement(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("dismantlement");
}

bool Dismantlement::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->isAllNude())
        return false;

    if(to_select == Self)
        return false;

    return true;
}

void Dismantlement::onEffect(const CardEffectStruct &effect) const{
    if(effect.to->isAllNude())
        return;

    Room *room = effect.to->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "hej", objectName());
    room->throwCard(card_id);

    LogMessage log;
    log.type = "$Dismantlement";
    log.from = effect.to;
    log.card_str = QString::number(card_id);
    room->sendLog(log);
}

Indulgence::Indulgence(Suit suit, int number)
    :DelayedTrick(suit, number)
{
    setObjectName("indulgence");
    target_fixed = false;

    judge.pattern = QRegExp("(.*):(heart):(.*)");
    judge.good = true;
    judge.reason = objectName();
}

bool Indulgence::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    if(!targets.isEmpty())
        return false;

    if(to_select->containsTrick(objectName()))
        return false;

    if(to_select == Self)
        return false;

    return true;
}

void Indulgence::takeEffect(ServerPlayer *target) const{
    Room *room = target->getRoom();
    room->broadcastInvoke("animate", QString("wuxingshan:%1").arg(target->objectName()));
    room->setEmotion(target, "wuxingshan");
    room->broadcastInvoke("playAudio", "wuxingshan");
    target->skip(Player::Play);
}

Disaster::Disaster(Card::Suit suit, int number)
    :DelayedTrick(suit, number, true)
{
    target_fixed = true;
}

bool Disaster::isAvailable(const Player *player) const{
    if(player->containsTrick(objectName()))
        return false;

    return ! player->isProhibited(player, this);
}

Lightning::Lightning(Suit suit, int number):Disaster(suit, number){
    setObjectName("lightning");

    judge.pattern = QRegExp("(.*):(spade):([2-9])");
    judge.good = false;
    judge.reason = objectName();
}

void Lightning::takeEffect(ServerPlayer *target) const{
    Room *room = target->getRoom();

    LogMessage log;
    log.type = "#Shengsibu";
    log.from = target;
    log.arg = QString::number(3);
    room->sendLog(log);
    room->loseHp(target, 3);

    room->broadcastInvoke("animate", QString("shengsibu:%1").arg(target->objectName()));
    room->setEmotion(target, "si");
}


// EX cards

class IceSwordSkill: public TriggerSkill{
public:
    IceSwordSkill():TriggerSkill("ice_sword"){
        events << SlashHit;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasWeapon(objectName());
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        Room *room = player->getRoom();

        if(!effect.to->isNude() && player->askForSkillInvoke("ice_sword", data)){
            int card_id = room->askForCardChosen(player, effect.to, "he", "ice_sword");
            room->throwCard(card_id);

            if(!effect.to->isNude()){
                card_id = room->askForCardChosen(player, effect.to, "he", "ice_sword");
                room->throwCard(card_id);
            }

            return true;
        }

        return false;
    }
};

IceSword::IceSword(Suit suit, int number)
    :Weapon(suit, number, 2)
{
    setObjectName("ice_sword");
    skill = new IceSwordSkill;
}

class RenwangShieldSkill: public ArmorSkill{
public:
    RenwangShieldSkill():ArmorSkill("renwang_shield"){
        events << SlashEffected;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->objectName() == "thunder_slash"){
            LogMessage log;
            log.type = "#ArmorNullify";
            log.from = player;
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            player->getRoom()->sendLog(log);

            return true;
        }else
            return false;
    }
};

QString RenwangShield::getEffectPath(bool ) const{
    return "audio/card/common/bishuizhu.ogg";
}

RenwangShield::RenwangShield(Suit suit, int number)
    :Armor(suit, number)
{
    setObjectName("renwang_shield");
    skill = new RenwangShieldSkill;
}

class BihuoShieldSkill: public ArmorSkill{
public:
    BihuoShieldSkill():ArmorSkill("bihuo_shield"){
        events << SlashEffected;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->objectName() == "fire_slash"){
            LogMessage log;
            log.type = "#ArmorNullify";
            log.from = player;
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            player->getRoom()->sendLog(log);

            return true;
        }else
            return false;
    }
};

QString BihuoShield::getEffectPath(bool ) const{
    return "audio/card/common/bihuozhao.ogg";
}

BihuoShield::BihuoShield(Suit suit, int number)
    :Armor(suit, number)
{
    setObjectName("bihuo_shield");
    skill = new BihuoShieldSkill;
}

class DingfengShieldSkill: public ArmorSkill{
public:
    DingfengShieldSkill():ArmorSkill("dingfeng_shield"){
        events << SlashEffected;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.nature == DamageStruct::Normal){
            LogMessage log;
            log.type = "#ArmorNullify";
            log.from = player;
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            player->getRoom()->sendLog(log);

            return true;
        }else
            return false;
    }
};

QString DingfengShield::getEffectPath(bool ) const{
    return "audio/card/common/dingfengdan.ogg";
}

DingfengShield::DingfengShield(Suit suit, int number)
    :Armor(suit, number)
{
    setObjectName("dingfeng_shield");
    skill = new DingfengShieldSkill;
}

class HorseSkill: public DistanceSkill{
public:
    HorseSkill():DistanceSkill("horse"){

    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        int correct = 0;
        if(from->getOffensiveHorse())
            correct += from->getOffensiveHorse()->getCorrect();
        if(to->getDefensiveHorse())
            correct += to->getDefensiveHorse()->getCorrect();

        return correct;
    }
};

QString Fenghuolun::getEffectPath(bool ) const{
    return "audio/card/common/fenghuolun.ogg";
}

Fenghuolun::Fenghuolun(Card::Suit suit, int number)
    :OffensiveHorse(suit, number)
{
  setObjectName("fenghuolun");
}

QString Bailongma::getEffectPath(bool ) const{
    return "audio/card/common/bailongma.ogg";
}

Bailongma::Bailongma(Card::Suit suit, int number)
    :OffensiveHorse(suit, number)
{
  setObjectName("bailongma");
}

QString Xiaotianquan::getEffectPath(bool ) const{
    return "audio/card/common/xiaotianquan.ogg";
}

Xiaotianquan::Xiaotianquan(Card::Suit suit, int number)
    :OffensiveHorse(suit, number)
{
  setObjectName("xiaotianquan");
}

QString Jindouyun::getEffectPath(bool ) const{
    return "audio/card/common/jindouyun.ogg";
}

Jindouyun::Jindouyun(Card::Suit suit, int number)
    :DefensiveHorse(suit, number)
{
  setObjectName("jindouyun");
}

QString Bishuishou::getEffectPath(bool ) const{
    return "audio/card/common/bishuishou.ogg";
}

Bishuishou::Bishuishou(Card::Suit suit, int number)
    :DefensiveHorse(suit, number)
{
  setObjectName("bishuishou");
}

NatureSlash::NatureSlash(Suit suit, int number, DamageStruct::Nature nature)
    :Slash(suit, number)
{
    this->nature = nature;
}

bool NatureSlash::match(const QString &pattern) const{
    if(pattern == "slash")
        return true;
    else
        return Slash::match(pattern);
}

ThunderSlash::ThunderSlash(Suit suit, int number)
    :NatureSlash(suit, number, DamageStruct::Thunder)
{
    setObjectName("thunder_slash");
}

FireSlash::FireSlash(Suit suit, int number)
    :NatureSlash(suit, number, DamageStruct::Fire)
{
    setObjectName("fire_slash");
    nature = DamageStruct::Fire;
}

class FanSkill: public WeaponSkill{
public:
    FanSkill():WeaponSkill("fan"){
        events << Predamage;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        ServerPlayer *bajiao = damage.from;
        Room *room = damage.to->getRoom();
        if(damage.card && damage.card->inherits("Slash")
            && damage.card->objectName() != "fire_slash"
            && damage.card->objectName() != "thunder_slash"
            && room->askForSkillInvoke(bajiao, "fan", data)){

            LogMessage log;
            log.type = "#BajiaoEffect";
            log.from = bajiao;
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

QString Fan::getEffectPath(bool ) const{
    return "audio/card/common/bajiaoshan.ogg";
}

Fan::Fan(Suit suit, int number):Weapon(suit, number, 3){
    setObjectName("fan");
    skill = new FanSkill;
}

class VineSkill: public ArmorSkill{
public:
    VineSkill():ArmorSkill("vine"){
        events << Predamaged << SlashEffected << CardEffected;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        if(event == SlashEffected){
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            if(effect.nature == DamageStruct::Normal || effect.nature == DamageStruct::Thunder){
                LogMessage log;
                log.from = player;
                log.type = "#ArmorNullify";
                log.arg = objectName();
                log.arg2 = effect.slash->objectName();
                player->getRoom()->sendLog(log);

                player->getRoom()->broadcastInvoke("playAudio", "jinlanjiasha");

                return true;
            }
        }else if(event == Predamaged){
            DamageStruct damage = data.value<DamageStruct>();
            if(damage.nature == DamageStruct::Fire){
                LogMessage log;
                log.type = "#VineDamage";
                log.from = player;
                log.arg = QString::number(damage.damage);
                log.arg2 = QString::number(damage.damage + 1);
                player->getRoom()->sendLog(log);

                damage.damage ++;
                data = QVariant::fromValue(damage);
            }
        }

        return false;
    }
};

QString Vine::getEffectPath(bool ) const{
    return "audio/card/common/jinlanjiasha.ogg";
}

Vine::Vine(Suit suit, int number):Armor(suit, number){
    setObjectName("vine");
    skill = new VineSkill;
}

SupplyShortage::SupplyShortage(Card::Suit suit, int number)
    :DelayedTrick(suit, number)
{
    setObjectName("supply_shortage");

    judge.pattern = QRegExp("(.*):(club):(.*)");
    judge.good = true;
    judge.reason = objectName();
}

bool SupplyShortage::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select == Self)
        return false;

    if(to_select->containsTrick(objectName()))
        return false;

    if(Self->hasSkill("qicai"))
        return true;

    int distance = Self->distanceTo(to_select);
    if(Self->hasSkill("duanliang"))
        return distance <= 2;
    else
        return distance <= 1;
}

void SupplyShortage::takeEffect(ServerPlayer *target) const{
    Room *room = target->getRoom();
    room->broadcastInvoke("animate", QString("jinguzhou:%1").arg(target->objectName()));
    room->setEmotion(target, "jinguzhou");
    room->broadcastInvoke("playAudio", "jinguzhou");
    target->skip(Player::Draw);
}

StandardCardPackage::StandardCardPackage()
    :Package("standard_cards")
{
    type = Package::CardPack;

    QList<Card*> cards;

    cards << new Slash(Card::Spade, 7)
          << new Slash(Card::Spade, 8)
          << new Slash(Card::Spade, 9)
          << new Slash(Card::Spade, 12)
          << new Slash(Card::Spade, 13)

          << new Slash(Card::Club, 2)
          << new Slash(Card::Club, 3)
          << new Slash(Card::Club, 4)
          << new Slash(Card::Club, 5)
          << new Slash(Card::Club, 6)
          << new Slash(Card::Club, 12)
          << new Slash(Card::Club, 9)
          << new Slash(Card::Club, 10)
          << new Slash(Card::Club, 11)
          << new Slash(Card::Club, 11)

          << new Jink(Card::Heart, 2)
          << new Jink(Card::Heart, 2)
          << new Jink(Card::Heart, 13)

          << new Jink(Card::Diamond, 2)
          << new Jink(Card::Diamond, 2)
          << new Jink(Card::Diamond, 3)
          << new Jink(Card::Diamond, 4)
          << new Jink(Card::Diamond, 5)
          << new Jink(Card::Diamond, 12)
          << new Jink(Card::Diamond, 7)
          << new Jink(Card::Diamond, 8)
          << new Jink(Card::Diamond, 9)
          << new Jink(Card::Diamond, 10)
          << new Jink(Card::Diamond, 11)
          << new Jink(Card::Diamond, 11)

          << new Peach(Card::Heart, 3)
          << new Peach(Card::Heart, 4)
          << new Peach(Card::Heart, 6)
          << new Peach(Card::Heart, 7)
          << new Peach(Card::Heart, 8)
          << new Peach(Card::Heart, 9)
          << new Peach(Card::Heart, 12)
          << new Peach(Card::Heart, 13)

          << new Peach(Card::Diamond, 12)

          << new Crossbow(Card::Club)
          << new Crossbow(Card::Diamond)
          << new DoubleSword
          << new QinggangSword
          << new Blade
          << new Spear
          << new Axe
          << new Halberd
          << new KylinBow

          << new EightDiagram(Card::Spade)
          << new EightDiagram(Card::Heart);

    skills << EightDiagramSkill::GetInstance();

    cards << new AmazingGrace(Card::Club, 3)
          << new AmazingGrace(Card::Heart, 4)
          << new GodSalvation
          << new SavageAssault(Card::Diamond, 9)
          << new SavageAssault(Card::Spade, 13)
          << new SavageAssault(Card::Club, 7)
          << new ArcheryAttack
          << new Duel(Card::Spade, 1)
          << new Duel(Card::Club, 1)
          << new Duel(Card::Spade, 1)
          << new ExNihilo(Card::Club, 13)
          << new ExNihilo(Card::Heart, 8)
          << new ExNihilo(Card::Heart, 9)
          << new ExNihilo(Card::Heart, 11)
          << new Snatch(Card::Spade, 3)
          << new Snatch(Card::Spade, 4)
          << new Snatch(Card::Club, 12)
          << new Snatch(Card::Spade, 3)
          << new Snatch(Card::Diamond, 4)
          << new Dismantlement(Card::Spade, 3)
          << new Dismantlement(Card::Spade, 4)
          << new Dismantlement(Card::Spade, 11)
          << new Dismantlement(Card::Club, 3)
          << new Dismantlement(Card::Club, 4)
          << new Dismantlement(Card::Heart, 12)
          << new Nullification(Card::Spade, 11)
          << new Nullification(Card::Diamond, 10)
          << new Nullification(Card::Club, 13)
          << new Indulgence(Card::Spade, 6)
          << new Indulgence(Card::Club, 5)
          << new Indulgence(Card::Heart, 6)
          << new Lightning(Card::Spade, 1)
          << new RenwangShield(Card::Spade, 6)
          << new Lightning(Card::Heart, 7)
          << new Nullification(Card::Spade, 12)
          << new DingfengShield(Card::Heart, 6)
          << new BihuoShield(Card::Diamond, 8)
          << new Jindouyun(Card::Club, 2)
          << new Bishuishou(Card::Diamond, 5)
          << new Bailongma(Card::Spade, 5)
          << new Xiaotianquan(Card::Club, 13)
          << new Fenghuolun(Card::Heart, 8);

    // spade
    cards << new Vine(Card::Spade, 2)
            << new ThunderSlash(Card::Spade, 4)
            << new ThunderSlash(Card::Spade, 5)
            << new ThunderSlash(Card::Spade, 6)
            << new ThunderSlash(Card::Spade, 3)
            << new ThunderSlash(Card::Spade, 1)
            << new ThunderSlash(Card::Spade, 9)
            << new ThunderSlash(Card::Spade, 9)
            << new ThunderSlash(Card::Spade, 10)
            << new SupplyShortage(Card::Spade,10)
            << new Nullification(Card::Spade, 13);

    // club
    cards << new Vine(Card::Club, 10)
            << new SupplyShortage(Card::Club, 1)
            << new ThunderSlash(Card::Club, 5)
            << new ThunderSlash(Card::Club, 6)
            << new ThunderSlash(Card::Club, 7)
            << new ThunderSlash(Card::Club, 11)
            << new ThunderSlash(Card::Club, 12)
            << new ThunderSlash(Card::Club, 1);

    // heart
    cards << new Nullification(Card::Heart, 1)
            << new FireSlash(Card::Heart, 1)
            << new Peach(Card::Heart, 5)
            << new Peach(Card::Heart, 6)
            << new FireSlash(Card::Heart, 7)
            << new FireSlash(Card::Heart, 10)
            << new FireSlash(Card::Heart, 10)
            << new FireSlash(Card::Heart, 11)
            << new Jink(Card::Heart, 2)
            << new Jink(Card::Heart, 9)
            << new FireSlash(Card::Heart, 10)
            << new Jink(Card::Heart, 11)
            << new Jink(Card::Heart, 12)
            << new Nullification(Card::Heart, 13);

    // diamond
    cards << new Fan(Card::Club, 8)
            << new Peach(Card::Diamond, 2)
            << new Peach(Card::Diamond, 3)
            << new FireSlash(Card::Diamond, 2)
            << new FireSlash(Card::Diamond, 5)
            << new FireSlash(Card::Diamond, 1)
            << new FireSlash(Card::Diamond, 7)
            << new FireSlash(Card::Diamond, 8)
            << new FireSlash(Card::Diamond, 9)
            << new FireSlash(Card::Diamond, 10)
            << new FireSlash(Card::Diamond, 13)
            << new Jink(Card::Diamond, 2)
            << new Jink(Card::Diamond, 7)
            << new Jink(Card::Diamond, 8)
            << new Jink(Card::Diamond, 13)
            << new Jink(Card::Diamond, 10)
            << new Jink(Card::Diamond, 11);

    foreach(Card *card, cards)
        card->setParent(this);

    skills << new SpearSkill << new AxeViewAsSkill << new HorseSkill;;
}

ADD_PACKAGE(StandardCard)
