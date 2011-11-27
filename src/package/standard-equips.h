#ifndef STANDARDEQUIPS_H
#define STANDARDEQUIPS_H

#include "standard.h"

class Crossbow:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Crossbow(Card::Suit suit, int number = 8);
    virtual QString getEffectPath(bool is_male) const;
};

class DoubleSword:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE DoubleSword(Card::Suit suit = Spade, int number = 6);
    virtual QString getEffectPath(bool is_male) const;
};

class QinggangSword:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE QinggangSword(Card::Suit suit = Club, int number = 7);
    virtual QString getEffectPath(bool is_male) const;
};

class Blade:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Blade(Card::Suit suit = Club, int number = 3);
    virtual QString getEffectPath(bool is_male) const;
};

class Spear:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Spear(Card::Suit suit = Spade, int number = 7);
    virtual QString getEffectPath(bool is_male) const;
};

class Axe:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Axe(Card::Suit suit = Club, int number = 6);
    virtual QString getEffectPath(bool is_male) const;
};

class Halberd:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Halberd(Card::Suit suit = Club, int number = 4);
    virtual QString getEffectPath(bool is_male) const;
};

class KylinBow:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE KylinBow(Card::Suit suit = Spade, int number = 4);
    virtual QString getEffectPath(bool is_male) const;
};

class EightDiagram:public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE EightDiagram(Card::Suit suit, int number = 5);
    virtual QString getEffectPath(bool is_male) const;
};

class IceSword: public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE IceSword(Card::Suit suit, int number);
};

class RenwangShield: public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE RenwangShield(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class BihuoShield: public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE BihuoShield(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class DingfengShield: public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE DingfengShield(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Fenghuolun: public OffensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Fenghuolun(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Bailongma: public OffensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Bailongma(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Xiaotianquan: public OffensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Xiaotianquan(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Jindouyun: public DefensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Jindouyun(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Bishuishou: public DefensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Bishuishou(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Fan: public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE Fan(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class Vine: public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE Vine(Card::Suit suit, int number);
    virtual QString getEffectPath(bool is_male) const;
};

class StandardCardPackage: public Package{
    Q_OBJECT

public:
    StandardCardPackage();
};

class StandardExCardPackage: public Package{
    Q_OBJECT

public:
    StandardExCardPackage();
};

#endif // STANDARDEQUIPS_H
