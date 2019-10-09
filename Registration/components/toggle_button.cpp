#include "stdafx.h"
#include "toggle_button.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

ToggleButton::ToggleButton(QWidget *parent)
    : QWidget(parent)
    , m_isChecked(false)
    , m_bgColorOff(QColor("#74797f"))
    , m_bgColorOn(QColor("#74797f"))
    , m_sliderColorEnabled(QColor("#0096d6"))
    , m_sliderColorDisabled(QColor("#0096d6"))
    , m_startX(0)
    , m_endX(0)
{
    m_step = width() / 20;

    m_timer = new QTimer(this);
    m_timer->setInterval(5);
    connect(m_timer, &QTimer::timeout, this, &ToggleButton::updateValue);
    connect(this, &ToggleButton::toggled, this, &ToggleButton::onToggled);
}

ToggleButton::~ToggleButton()
{
}

void ToggleButton::mouseReleaseEvent(QMouseEvent *)
{
    m_isChecked = !m_isChecked;
    emit toggled(m_isChecked);
}

void ToggleButton::resizeEvent(QResizeEvent *)
{
    m_startX = m_isChecked ? width() - height() : 0;
    update();
}

void ToggleButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBg(&painter);
    drawSlider(&painter);

    QImage image_left("://icons/icon-gridview-hover.png");
    QImage image_right("://icons/icon-picinpic-hover.png");

    QRectF rect_left(10,10,20,20);
    painter.drawImage(rect_left, image_left);
    QRectF rect_right(width()-30,10,20,20);
    painter.drawImage(rect_right, image_right);
}

void ToggleButton::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    painter->setBrush(m_isChecked? m_bgColorOn : m_bgColorOff);

    QRect rect(0, 0, width(), height());
    int radius = rect.height() / 2;
    int circleWidth = rect.height();

    QPainterPath path;
    path.moveTo(radius, rect.left());
    path.arcTo(QRectF(rect.left(), rect.top(), circleWidth, circleWidth), 90, 180);
    path.lineTo(rect.width() - radius, rect.height());
    path.arcTo(QRectF(rect.width() - rect.height(), rect.top(), circleWidth, circleWidth), 270, 180);
    path.lineTo(radius, rect.top());

    painter->drawPath(path);

    painter->restore();
}

void ToggleButton::drawSlider(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    painter->setBrush(isEnabled() ? m_sliderColorEnabled : m_sliderColorDisabled);

    QRect rect(0, 0, width(), height());
    int sliderWidth = rect.height();
    QRect sliderRect(m_startX, 0, sliderWidth, sliderWidth);
    painter->drawEllipse(sliderRect);

    painter->restore();
}

void ToggleButton::updateValue()
{
    if (m_isChecked) {
        if (m_startX < m_endX) {
            m_startX = m_startX + m_step;
        } else {
            m_startX = m_endX;
            m_timer->stop();
        }
    } else {
        if (m_startX > m_endX) {
            m_startX = m_startX - m_step;
        } else {
            m_startX = m_endX;
            m_timer->stop();
        }
    }

    update();
}

void ToggleButton::onToggled()
{
    m_endX = m_isChecked ? width() - height() : 0;
    m_timer->start();
}

void ToggleButton::setChecked(bool checked)
{
    if (m_isChecked != checked) {
        m_isChecked = checked;
        emit toggled(m_isChecked);
        update();
    }
}

void ToggleButton::setBgColor(QColor bgColorOff, QColor bgColorOn)
{
    m_bgColorOff = bgColorOff;
    m_bgColorOn = bgColorOn;
    update();
}

void ToggleButton::setSliderColor(QColor sliderColorEnabled, QColor sliderColorDisabled)
{
    m_sliderColorEnabled = sliderColorEnabled;
    m_sliderColorDisabled = sliderColorDisabled;
    update();
}
