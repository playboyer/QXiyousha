#include "indicatoritem.h"
#include "engine.h"

#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QPauseAnimation>

IndicatorItem::IndicatorItem(const QPointF &start, const QPointF &real_finish, Player *player)
    :start(start), finish(start), real_finish(real_finish)
{
    QGraphicsDropShadowEffect *halo = new QGraphicsDropShadowEffect();
    halo->setOffset(0,0);
    halo->setBlurRadius(5);
    halo->setColor(Qt::white);

    //setGraphicsEffect(halo);
    color = Sanguosha->getKingdomColor(player->getKingdom());
    width = player->isLord() ? 3 : 2;
}

void IndicatorItem::doAnimation(){
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(this);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "finish");
    animation->setEndValue(real_finish);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setDuration(800);

    QPropertyAnimation *pause = new QPropertyAnimation(this,"opacity");
    pause->setEndValue(0);
    pause->setEasingCurve(QEasingCurve::InQuart);

    group->addAnimation(animation);
    group->addAnimation(pause);

    group->start(QAbstractAnimation::DeleteWhenStopped);

    connect(group, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QPointF IndicatorItem::getFinish() const{
    return finish;
}

void IndicatorItem::setFinish(const QPointF &finish){
    this->finish = finish;
    update();
}

void IndicatorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPen pen(color);
    pen.setWidthF(width);

    int baseX = qMin(start.x(),finish.x());
    int baseY = qMin(start.y(),finish.y());

    QLinearGradient linearGrad(start - QPoint(baseX,baseY),
                               finish - QPoint(baseX,baseY));
    linearGrad.setColorAt(0, color.darker());
    linearGrad.setColorAt(1, Qt::white);


    QBrush brush(linearGrad);
    pen.setBrush(brush);

    painter->setPen(pen);
    painter->drawLine(mapFromScene(start), mapFromScene(finish));
}

QRectF IndicatorItem::boundingRect() const{
    qreal width = qAbs(start.x() - real_finish.x());
    qreal height = qAbs(start.y() - real_finish.y());

    return QRectF(0, 0, width, height);
}
