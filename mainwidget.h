#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QSpinBox>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QTextCodec>
#include <QTranslator>
#include <QMouseEvent>
#include <QDateTime>
#include <QLocale>
#include <QTimer>
#include <QFile>



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE



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
    QVector<QSpinBox *> enemyPropSpinBoxVec;

    QTimer playerAutoPlayTimer;
    QTimer enemyAutoPlayTimer;

    QString buildDateStr;
    QString buildTimeStr;

    QByteArray newEngineBA;

    QByteArray engineBA;
    QByteArray libraryBA;

    QVector<int> playerIndexVec;
    QVector<int> npcIndexVec;
    QVector<int> enemyIndexVec;
    QVector<int> sceneObjIndexVec;

    QVector<QVector<int>> propIndexVecVec;
    QVector<QVector<int>> magicIndexVecVec;

    QVector<QVector<int>> roleIconIndexVecVec;
    QVector<QVector<int>> propIconIndexVecVec;

    int8_t fromNumber(uint8_t num) {
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

    void Delay_ms(uint32_t ms) {
        QTime dstTime = QTime::currentTime().addMSecs(ms);
        while(QTime::currentTime() < dstTime) {
            QApplication::processEvents(QEventLoop::AllEvents);
        }
    }

private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textChanged();

    void on_playerIdx_valueChanged(int index);
    void on_playerAutoPlayChk_clicked();
    void on_playerAutoPlayTimer_timeout();
    void on_playerIcon_valueChanged(int index);
    void on_playerToward_currentIndexChanged(int index);
    void on_playerFootstep_valueChanged(int index);
    void on_playerData_valueChanged();

    void on_enemyIdx_valueChanged(int index);
    void on_enemyAutoPlayChk_clicked();
    void on_enemyAutoPlayTimer_timeout();
    void on_enemyIcon_valueChanged(int index);
    void on_enemyData_valueChanged();

    void on_propType_currentIndexChanged(int index);
    void on_propIdx_valueChanged(int index);
    void on_propIcon_valueChanged(int index);
    void on_propData_valueChanged();
    void on_propDescription_textChanged();

    void on_magicType_currentIndexChanged(int index);
    void on_magicIdx_valueChanged(int index);
    void on_magicData_valueChanged();
    void on_magicDescription_textChanged();


};
#endif // MAINWIDGET_H
