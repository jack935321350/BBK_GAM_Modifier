#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QSpinBox>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     toward;
    uint8_t     foottep;
    uint8_t     action;
    uint8_t     pos_x;
    uint8_t     pos_y;
    uint8_t     reserved1;
    uint8_t     reserved2;
    uint8_t     learned;
    uint8_t     name[12];
    uint8_t     icon;
    uint8_t     magic;
    uint8_t     hat;
    uint8_t     clothes;
    uint8_t     cloak;
    uint8_t     wristlet;
    uint8_t     weapon;
    uint8_t     shoes;
    uint8_t     accessory1;
    uint8_t     accessory2;
    uint8_t     level;
    uint8_t     immunity;
    uint8_t     enchanting;
    uint8_t     multiplayer_magic;
    uint8_t     hp_each_turn;
    uint8_t     mp_each_turn;
    uint16_t    hp_max;
    uint16_t    hp;
    uint16_t    mp_max;
    uint16_t    mp;
    uint16_t    attack_power;
    uint16_t    defense_power;
    uint16_t    exp;
    uint16_t    exp_level_up;
    uint8_t     body_movement;
    uint8_t     spirit_power;
    uint8_t     lucky_value;
    uint8_t     abnormal_round;

} player_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     reserved1;
    uint8_t     who_can_use;
    uint8_t     effective_round;
    uint8_t     icon;
    uint8_t     name[12];
    uint16_t    price_buy;
    uint16_t    price_sale;

    union {
        uint8_t mpMax;
        uint8_t mp;
    }           mpMax_mp;

    union {
        uint8_t hpMax;
        uint8_t hp;
    }           hpMax_hp;

    uint8_t     defense_power;
    uint8_t     attack_power;

    union {
        uint8_t spirit_power;
        uint8_t index;
    }           spirit_power_index;

    union {
        uint8_t body_movement;
        uint8_t animation;
    }           body_movement_animation;

    union {
        uint8_t effective;
        uint8_t multiplayer_magic;
    }           effective_multiplayer_magic;

    union {
        uint8_t lucky_value;
        uint8_t unknown;
    }           lucky_value_unknown;

    uint8_t     description[102];
    uint16_t    event;

} prop_t;



class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWidget *ui;
    QVector<QSpinBox *> playerPropSpinBoxVec;

    QByteArray newEngineBA;

    QByteArray engineBA;
    QByteArray libraryBA;

    QVector<int> playerIndexVec;
    QVector<QVector<int>> propIndexVecVec;

    int8_t fromNumber(uint8_t num){
        if(num & 0x80) {
            return -(num & 0x7f);
        }
        return num;
    }

    uint8_t toNumber(int8_t num) {
        if(num < 0) {
            return (-num | 0x80);
        }
        return num;
    }

private slots:
    void on_buttons_clicked();

    void on_playerIdx_valueChanged(int index);
    void on_playerData_valueChanged();

    void on_propType_currentIndexChanged(int index);
    void on_propIdx_valueChanged(int index);
    void on_propData_valueChanged();

    void on_propDescription_textChanged();
};
#endif // MAINWIDGET_H
