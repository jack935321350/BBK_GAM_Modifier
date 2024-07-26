#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    static QTranslator translator;
    translator.load(":/translations/zh_CN.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    // adjustSize();
    resize(566, 576);

    QFile newEngineFile(":/newEngine.head");
    if(newEngineFile.open(QFile::ReadOnly)) {
        newEngineBA = newEngineFile.readAll();
        newEngineFile.close();
    }

    playerPropSpinBoxVec.append(ui->playerHat);
    playerPropSpinBoxVec.append(ui->playerClothes);
    playerPropSpinBoxVec.append(ui->playerCloak);
    playerPropSpinBoxVec.append(ui->playerWristlet);
    playerPropSpinBoxVec.append(ui->playerWeapon);
    playerPropSpinBoxVec.append(ui->playerShoes);
    playerPropSpinBoxVec.append(ui->playerAccessory1);
    playerPropSpinBoxVec.append(ui->playerAccessory2);

    connect(ui->loadGamBtn,         &QPushButton::clicked,          this, &MainWidget::on_buttons_clicked);
    connect(ui->replaceNewEngine,   &QPushButton::clicked,          this, &MainWidget::on_buttons_clicked);
    connect(ui->allPlayer999Btn,    &QPushButton::clicked,          this, &MainWidget::on_buttons_clicked);
    connect(ui->saveGamBtn,         &QPushButton::clicked,          this, &MainWidget::on_buttons_clicked);

    ui->playerHat->installEventFilter(this);
    ui->playerClothes->installEventFilter(this);
    ui->playerCloak->installEventFilter(this);
    ui->playerWristlet->installEventFilter(this);
    ui->playerWeapon->installEventFilter(this);
    ui->playerShoes->installEventFilter(this);
    ui->playerAccessory1->installEventFilter(this);
    ui->playerAccessory2->installEventFilter(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // qDebug() << size();
}

bool MainWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        QSpinBox *spinBox = static_cast<QSpinBox *>(obj);
        if(spinBox == ui->playerHat) {
            ui->propType->setCurrentIndex(0);
        }
        else if(spinBox == ui->playerClothes) {
            ui->propType->setCurrentIndex(1);
        }
        else if(spinBox == ui->playerCloak) {
            ui->propType->setCurrentIndex(3);
        }
        else if(spinBox == ui->playerWristlet) {
            ui->propType->setCurrentIndex(4);
        }
        else if(spinBox == ui->playerWeapon) {
            ui->propType->setCurrentIndex(6);
        }
        else if(spinBox == ui->playerShoes) {
            ui->propType->setCurrentIndex(2);
        }
        else if(spinBox == ui->playerAccessory1 || spinBox == ui->playerAccessory2) {
            ui->propType->setCurrentIndex(5);
        }
        ui->propIdx->setValue(spinBox->value());
    }
    return QWidget::eventFilter(obj, event);
}

void MainWidget::on_buttons_clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());

    if(btn == ui->loadGamBtn) {

        QSettings settings("QtProject", "BBK_GAM_Modifier");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("last_dir").toString(), "GAM(*.gam)");
        if(!fileName.isEmpty() && QFile(fileName).exists()) {
            settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
            settings.sync();
        }
        else {
            return;
        }

        setWindowTitle(tr("BBK_GAM_Modifier"));

        engineBA.clear();
        libraryBA.clear();
        playerIndexVec.clear();
        propIndexVecVec.clear();

        QFile gamFile(fileName);
        if(!gamFile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam File Open Failed!"));
            return;
        }
        QByteArray gamBA = gamFile.readAll();
        gamFile.close();

        int libIndex = gamBA.indexOf(QByteArray("LIB", 3));
        if(libIndex < 0 || libIndex > (gamFile.size() - 5)) {
            QMessageBox::warning(this, tr("Warning"), tr("Tag LIB Seek Failed!"));
            return;
        }

        engineBA = gamBA.left(libIndex);
        libraryBA = gamBA.mid(libIndex);

        int arsIndex = libraryBA.indexOf(QByteArray("ARS", 3));
        if(arsIndex < 0 || arsIndex > (libraryBA.size() - 5)) {
            QMessageBox::warning(this, tr("Warning"), tr("Tag ARS Seek Failed!"));
            return;
        }

        int playerIndex = arsIndex + 16;
        do {
            playerIndexVec.append(playerIndex);
            playerIndex += sizeof(player_t);
        } while(libraryBA.at(playerIndex) == 0x01);
        ui->playerIdx->setRange(1, playerIndexVec.size());
        on_playerIdx_valueChanged(ui->playerIdx->value());

        QVector<int> grsIndexVec;
        int grsIndex = libraryBA.indexOf(QByteArray("GRS", 3));
        if(grsIndex < 0 || grsIndex > (libraryBA.size() - 5)) {
            QMessageBox::warning(this, tr("Warning"), tr("Tag GRS Seek Failed!"));
            return;
        }
        do {
            grsIndexVec.append(grsIndex);
            grsIndex = libraryBA.indexOf(QByteArray("GRS", 3), grsIndexVec.last() + 3);
        } while(grsIndex > 0);

        QVector<int> propIndexVec;
        for(int i = 0; i < grsIndexVec.size(); i++) {
            grsIndex = grsIndexVec[i];
            int propIndex = grsIndex + 16;
            do {
                uint8_t propType = libraryBA.at(propIndex);
                if(propType == 0x00 || propType == 0xff) {
                    break;
                }
                if(propType - 1 > propIndexVecVec.size()) {
                    propIndexVecVec.append(propIndexVec);
                    propIndexVec.clear();
                }
                propIndexVec.append(propIndex);
                propIndex += sizeof(prop_t);
            } while((grsIndex + 0x4000 - propIndex) > sizeof(prop_t));
        }
        if(!propIndexVec.isEmpty()) {
            propIndexVecVec.append(propIndexVec);
        }

        ui->playerHat->setRange(0, propIndexVecVec[0].size());
        ui->playerClothes->setRange(0, propIndexVecVec[1].size());
        ui->playerCloak->setRange(0, propIndexVecVec[3].size());
        ui->playerWristlet->setRange(0, propIndexVecVec[4].size());
        ui->playerWeapon->setRange(0, propIndexVecVec[6].size());
        ui->playerShoes->setRange(0, propIndexVecVec[2].size());
        ui->playerAccessory1->setRange(0, propIndexVecVec[5].size());
        ui->playerAccessory2->setRange(0, propIndexVecVec[5].size());


        on_propType_currentIndexChanged(ui->propType->currentIndex());

        setWindowTitle(QFileInfo(fileName).fileName());

    }
    else if(btn == ui->replaceNewEngine) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam Data Not Loaded!"));
            return;
        }

        engineBA = newEngineBA;
        QMessageBox::information(this, tr("Information"), tr("New Engine Replace OK!"));

    }
    else if(btn == ui->allPlayer999Btn) {

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam Data Not Loaded!"));
            return;
        }

        for(int i = 0; i < playerIndexVec.size(); i++) {
            player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[i]);
            player->attack_power = 999;
            player->defense_power = 999;
            player->body_movement = 99;
            player->spirit_power = 20;
            player->lucky_value = 99;
            player->hp_max = 999;
            player->hp = 999;
            player->mp_max = 999;
            player->mp = 999;
        }
        on_playerIdx_valueChanged(ui->playerIdx->value());

    }
    else if(btn == ui->saveGamBtn){

        if(engineBA.isEmpty() || libraryBA.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam Data Not Loaded!"));
            return;
        }

        QSettings settings("QtProject", "BBK_GAM_Modifier");
        QFile gamFile(QFileDialog::getSaveFileName(this, tr("Save File"), settings.value("last_dir").toString(), "GAM(*.gam)"));
        if(!gamFile.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Warning"), tr("Gam File Save Failed!"));
            return;
        }

        QDataStream gamStream(&gamFile);
        gamStream.writeRawData(engineBA.data(), engineBA.size());
        gamStream.writeRawData(libraryBA.data(), libraryBA.size());

        gamFile.close();

    }
}



void MainWidget::on_playerIdx_valueChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    disconnect(ui->playerName,          &QLineEdit::editingFinished,    this, &MainWidget::on_playerData_valueChanged);
    disconnect(ui->playerHat,           SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerClothes,       SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerCloak,         SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerWristlet,      SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerWeapon,        SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerShoes,         SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerAccessory1,    SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    disconnect(ui->playerAccessory2,    SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));

    player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[index - 1]);
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    ui->playerName->setText(gbkCodec->toUnicode((char *)player->name));
    ui->playerHat->setValue(player->hat);
    ui->playerClothes->setValue(player->clothes);
    ui->playerCloak->setValue(player->cloak);
    ui->playerWristlet->setValue(player->wristlet);
    ui->playerWeapon->setValue(player->weapon);
    ui->playerShoes->setValue(player->shoes);
    ui->playerAccessory1->setValue(player->accessory1);
    ui->playerAccessory2->setValue(player->accessory2);
    ui->playerAttackPower->setValue(player->attack_power);
    ui->playerDefensePower->setValue(player->defense_power);
    ui->playerHpMax->setValue(player->hp_max);
    ui->playerHp->setValue(player->hp);
    ui->playerMpMax->setValue(player->mp_max);
    ui->playerMp->setValue(player->mp);
    ui->playerBodyMovement->setValue(player->body_movement);
    ui->playerSpiritPower->setValue(player->spirit_power);
    ui->playerLuckyValue->setValue(player->lucky_value);

    connect(ui->playerName,         &QLineEdit::editingFinished,    this, &MainWidget::on_playerData_valueChanged);
    connect(ui->playerHat,          SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerClothes,      SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerCloak,        SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerWristlet,     SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerWeapon,       SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerShoes,        SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerAccessory1,   SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
    connect(ui->playerAccessory2,   SIGNAL(valueChanged(int)),      this, SLOT(on_playerData_valueChanged()));
}

void MainWidget::on_playerData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    for(int i = 0; i < playerPropSpinBoxVec.size(); i++) {
        if(sender() == playerPropSpinBoxVec[i]) {
            QSpinBox *spinBox = static_cast<QSpinBox *>(sender());
            ui->propIdx->setValue(spinBox->value());
        }
    }

    player_t *player = reinterpret_cast<player_t *>(libraryBA.data() + playerIndexVec[ui->playerIdx->value() - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray playerName = gbkCodec->fromUnicode(ui->playerName->text());
    memset(player->name, 0xff, 12);
    memcpy_s(player->name, 12, playerName.data(), playerName.size());
    player->name[playerName.size()] = '\0';

    player->hat             = ui->playerHat->value();
    player->clothes         = ui->playerClothes->value();
    player->cloak           = ui->playerCloak->value();
    player->wristlet        = ui->playerWristlet->value();
    player->weapon          = ui->playerWeapon->value();
    player->shoes           = ui->playerShoes->value();
    player->accessory1      = ui->playerAccessory1->value();
    player->accessory2      = ui->playerAccessory2->value();
    player->attack_power    = ui->playerAttackPower->value();
    player->defense_power   = ui->playerDefensePower->value();
    player->hp_max          = ui->playerHpMax->value();
    player->hp              = ui->playerHp->value();
    player->mp_max          = ui->playerMpMax->value();
    player->mp              = ui->playerMp->value();
    player->body_movement   = ui->playerBodyMovement->value();
    player->spirit_power    = ui->playerSpiritPower->value();
    player->lucky_value     = ui->playerLuckyValue->value();
}



void MainWidget::on_propType_currentIndexChanged(int index)
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    disconnect(ui->propName,            &QLineEdit::editingFinished,    this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->propHp,              SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propMp,              SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propHpMax,           SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propMpMax,           SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propAttackPower,     SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propDefensePower,    SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propBodyMovement,    SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propSpiritPower,     SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propLuckyValue,      SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->player1Chk,          &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player2Chk,          &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player3Chk,          &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->player4Chk,          &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->multiplayerMagic,    SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propEvent,           SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->poisonChk,           &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->messChk,             &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->sealChk,             &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->sleepChk,            &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->groupEffectChk,      &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    disconnect(ui->effectiveRound,      SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    disconnect(ui->propDescription,     &QTextEdit::textChanged,        this, &MainWidget::on_propData_valueChanged);

    if(propIndexVecVec[index].isEmpty()) {
        ui->propIdx->setRange(0, 0);
        return;
    }

    ui->propIdx->setRange(1, propIndexVecVec[index].size());
    prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[index][ui->propIdx->value() - 1]);
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    ui->propName->setText(gbkCodec->toUnicode((char *)prop->name));
    if(index < 5) {
        ui->propHp->setEnabled(false);
        ui->propHpMax->setEnabled(true);
        ui->propHpMax->setValue(fromNumber(prop->hpMax_hp.hpMax));

        ui->propMp->setEnabled(false);
        ui->propMpMax->setEnabled(true);
        ui->propMpMax->setValue(fromNumber(prop->mpMax_mp.mpMax));

        ui->propAttackPower->setValue(fromNumber(prop->attack_power));

        ui->poisonChk->setEnabled(true);
        ui->messChk->setEnabled(true);
        ui->sealChk->setEnabled(true);
        ui->sleepChk->setEnabled(true);
        ui->poisonChk->setChecked(prop->effective_multiplayer_magic.effective & 0x08);
        ui->messChk->setChecked(prop->effective_multiplayer_magic.effective & 0x04);
        ui->sealChk->setChecked(prop->effective_multiplayer_magic.effective & 0x02);
        ui->sleepChk->setChecked(prop->effective_multiplayer_magic.effective & 0x01);

        ui->multiplayerMagic->setEnabled(false);

        ui->groupEffectChk->setEnabled(false);
        ui->effectiveRound->setEnabled(false);
    }
    else if(index < 6) {
        ui->propHp->setEnabled(true);
        ui->propHp->setValue(fromNumber(prop->hpMax_hp.hp));
        ui->propHpMax->setEnabled(false);

        ui->propMp->setEnabled(true);
        ui->propMp->setValue(fromNumber(prop->mpMax_mp.mp));
        ui->propMpMax->setEnabled(false);

        ui->propAttackPower->setValue(fromNumber(prop->attack_power));

        ui->poisonChk->setEnabled(false);
        ui->messChk->setEnabled(false);
        ui->sealChk->setEnabled(false);
        ui->sleepChk->setEnabled(false);

        ui->multiplayerMagic->setEnabled(true);
        ui->multiplayerMagic->setValue(prop->effective_multiplayer_magic.multiplayer_magic);

        ui->groupEffectChk->setEnabled(false);
        ui->effectiveRound->setEnabled(false);
    }
    else if(index < 7) {
        ui->propHp->setEnabled(false);
        ui->propHpMax->setEnabled(true);
        ui->propHpMax->setValue(fromNumber(prop->hpMax_hp.hpMax));

        ui->propMp->setEnabled(false);
        ui->propMpMax->setEnabled(true);
        ui->propMpMax->setValue(fromNumber(prop->mpMax_mp.mpMax));

        ui->propAttackPower->setValue(prop->attack_power);

        ui->poisonChk->setEnabled(true);
        ui->messChk->setEnabled(true);
        ui->sealChk->setEnabled(true);
        ui->sleepChk->setEnabled(true);
        ui->poisonChk->setChecked(prop->effective_multiplayer_magic.effective & 0x08);
        ui->messChk->setChecked(prop->effective_multiplayer_magic.effective & 0x04);
        ui->sealChk->setChecked(prop->effective_multiplayer_magic.effective & 0x02);
        ui->sleepChk->setChecked(prop->effective_multiplayer_magic.effective & 0x01);

        ui->multiplayerMagic->setEnabled(false);

        ui->groupEffectChk->setEnabled(true);
        ui->groupEffectChk->setChecked(prop->effective_multiplayer_magic.effective & 0x10);

        ui->effectiveRound->setEnabled(true);
        ui->effectiveRound->setValue(prop->effective_round);
    }

    ui->propDefensePower->setValue(fromNumber(prop->defense_power));

    ui->propBodyMovement->setValue(fromNumber(prop->body_movement_animation.body_movement));
    ui->propSpiritPower->setValue(fromNumber(prop->spirit_power_index.spirit_power));
    ui->propLuckyValue->setValue(fromNumber(prop->lucky_value_unknown.lucky_value));

    ui->player1Chk->setChecked(prop->who_can_use & 0x01);
    ui->player2Chk->setChecked(prop->who_can_use & 0x02);
    ui->player3Chk->setChecked(prop->who_can_use & 0x04);
    ui->player4Chk->setChecked(prop->who_can_use & 0x08);

    ui->propEvent->setValue(prop->event);

    ui->propDescription->setText(gbkCodec->toUnicode((char *)prop->description));

    connect(ui->propName,           &QLineEdit::editingFinished,    this, &MainWidget::on_propData_valueChanged);
    connect(ui->propHp,             SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propMp,             SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propHpMax,          SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propMpMax,          SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propAttackPower,    SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propDefensePower,   SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propBodyMovement,   SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propSpiritPower,    SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propLuckyValue,     SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->player1Chk,         &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player2Chk,         &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player3Chk,         &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->player4Chk,         &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->multiplayerMagic,   SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propEvent,          SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->poisonChk,          &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->messChk,            &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->sealChk,            &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->sleepChk,           &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->groupEffectChk,     &QCheckBox::clicked,            this, &MainWidget::on_propData_valueChanged);
    connect(ui->effectiveRound,     SIGNAL(valueChanged(int)),      this, SLOT(on_propData_valueChanged()));
    connect(ui->propDescription,    &QTextEdit::textChanged,        this, &MainWidget::on_propData_valueChanged);
}

void MainWidget::on_propIdx_valueChanged(int index)
{
    Q_UNUSED(index)
    on_propType_currentIndexChanged(ui->propType->currentIndex());
}

void MainWidget::on_propData_valueChanged()
{
    if(engineBA.isEmpty() || libraryBA.isEmpty()) {
        return;
    }

    prop_t *prop = reinterpret_cast<prop_t *>(libraryBA.data() + propIndexVecVec[ui->propType->currentIndex()][ui->propIdx->value() - 1]);

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");

    QByteArray propName = gbkCodec->fromUnicode(ui->propName->text());
    memset(prop->name, 0xff, 12);
    memcpy_s(prop->name, 12, propName.data(), propName.size());
    prop->name[propName.size()] = '\0';

    if(ui->propType->currentIndex() < 5) {
        prop->hpMax_hp.hpMax = toNumber(ui->propHpMax->value());
        prop->mpMax_mp.mpMax = toNumber(ui->propMpMax->value());

        prop->attack_power = toNumber(ui->propAttackPower->value());

        if(ui->poisonChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x08;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x08;
        }

        if(ui->messChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x04;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x04;
        }

        if(ui->sealChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x02;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x02;
        }

        if(ui->sleepChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x01;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x01;
        }
    }
    else if(ui->propType->currentIndex() < 6) {
        prop->hpMax_hp.hp = toNumber(ui->propHp->value());
        prop->mpMax_mp.mp = toNumber(ui->propMp->value());

        prop->attack_power = toNumber(ui->propAttackPower->value());

        prop->effective_multiplayer_magic.multiplayer_magic = ui->multiplayerMagic->value();
    }
    else if(ui->propType->currentIndex() < 7) {
        prop->hpMax_hp.hpMax = toNumber(ui->propHpMax->value());
        prop->mpMax_mp.mpMax = toNumber(ui->propMpMax->value());

        prop->attack_power = ui->propAttackPower->value();

        if(ui->poisonChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x08;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x08;
        }

        if(ui->messChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x04;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x04;
        }

        if(ui->sealChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x02;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x02;
        }

        if(ui->sleepChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x01;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x01;
        }

        if(ui->groupEffectChk->isChecked()) {
            prop->effective_multiplayer_magic.effective |= 0x10;
        } else {
            prop->effective_multiplayer_magic.effective &= ~0x10;
        }

        prop->effective_round = ui->effectiveRound->value();
    }

    prop->defense_power = toNumber(ui->propDefensePower->value());

    prop->body_movement_animation.body_movement = toNumber(ui->propBodyMovement->value());
    prop->spirit_power_index.spirit_power       = toNumber(ui->propSpiritPower->value());
    prop->lucky_value_unknown.lucky_value       = toNumber(ui->propLuckyValue->value());

    if(ui->player1Chk->isChecked()) {
        prop->who_can_use |= 0x01;
    } else {
        prop->who_can_use &= ~0x01;
    }

    if(ui->player2Chk->isChecked()) {
        prop->who_can_use |= 0x02;
    } else {
        prop->who_can_use &= ~0x02;
    }

    if(ui->player3Chk->isChecked()) {
        prop->who_can_use |= 0x04;
    } else {
        prop->who_can_use &= ~0x04;
    }

    if(ui->player4Chk->isChecked()) {
        prop->who_can_use |= 0x08;
    } else {
        prop->who_can_use &= ~0x08;
    }

    prop->event = ui->propEvent->value();

    QByteArray propDescription = gbkCodec->fromUnicode(ui->propDescription->toPlainText());
    memset(prop->description, 0xff, 102);
    memcpy_s(prop->description, 102, propDescription.data(), propDescription.size());
    prop->description[propDescription.size()] = '\0';
}

void MainWidget::on_propDescription_textChanged()
{
    int len8Bit = ui->propDescription->toPlainText().toLocal8Bit().length();
    if(len8Bit > 100) {
        ui->propDescription->textCursor().movePosition(QTextCursor::End);
        if(ui->propDescription->textCursor().hasSelection()) {
            ui->propDescription->textCursor().clearSelection();
        }
        do {
            ui->propDescription->textCursor().deletePreviousChar();
            len8Bit = ui->propDescription->toPlainText().toLocal8Bit().length();
        } while(len8Bit > 100);
    }
}

