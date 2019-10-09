#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include <QWidget>
#include <QTimer>

class ToggleButton: public QWidget
{
    Q_OBJECT

public:
    ToggleButton(QWidget *parent = 0);
    ~ToggleButton();

public:
    bool isChecked()const { return m_isChecked; }

public slots:
    void setChecked(bool checked);
    void setBgColor(QColor bgColorOff, QColor bgColorOn);
    void setSliderColor(QColor sliderColorEnabled, QColor sliderColorDisabled);

protected:
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawSlider(QPainter *painter);

signals:
    void toggled(bool checked);

private slots:
    void updateValue();
    void onToggled();

private:
    bool m_isChecked;

    QColor m_bgColorOff;
    QColor m_bgColorOn;

    QColor m_sliderColorEnabled;
    QColor m_sliderColorDisabled;

    int m_step;
    int m_startX;
    int m_endX;
    QTimer* m_timer;
};

#endif // TOGGLE_BUTTON_H
