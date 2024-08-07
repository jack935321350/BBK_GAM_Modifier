#include "iconviewedit.h"
#include "g_fun.h"

IconViewEdit::IconViewEdit(QWidget *parent) : QWidget{parent}
{
    connect(this, SIGNAL(posBlack(QPoint)), this, SLOT(on_posBlack(QPoint)));
    connect(this, SIGNAL(posWhite(QPoint)), this, SLOT(on_posWhite(QPoint)));
    connect(this, SIGNAL(posTransparent(QPoint)), this, SLOT(on_posTransparent(QPoint)));
}

void IconViewEdit::ClearView(void)
{
    _scale = 1.0;
    _IconW = 0;
    _IconH = 0;
    _IconCnt = 0;
    _IconTransparency = 0;
    memset(_IconData, 0, ICON_DATA_MAX_SIZE);
    memset(_IconSrcData, 0, ICON_DATA_MAX_SIZE);
    update();
}

void IconViewEdit::UpdateView(uint8_t *data, uint32_t len)
{
    int32_t iconSrcW    = data[2];
    _IconW              = data[2];
    if(_IconW % 8 > 0) {
        _IconW += (8 - _IconW % 8);
    }
    _IconH              = data[3];
    _IconCnt            = data[4];
    _IconTransparency   = data[5];
    _DataSize = 6 + _IconW * _IconH * _IconCnt * _IconTransparency / 8;
    if(_DataSize != len) {
        _DataSize = 0;
        return;
    }
    memcpy_s(_IconData, ICON_DATA_MAX_SIZE, data + 6, len - 6);
    memcpy_s(_IconSrcData, ICON_DATA_MAX_SIZE, data + 6, len - 6);
    if(iconSrcW < _IconW) {
        if(_IconTransparency < 2) {
            int bytesPerRow = _IconW / 8;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 8;
                    _IconData[index] = _IconData[index] & ~(1 << (7 - (x % 8)));
                    _IconSrcData[index] = _IconData[index] & ~(1 << (7 - (x % 8)));
                }
            }
        }
        else if(_IconTransparency < 3) {
            int bytesPerRow = _IconW / 4;
            for(int y = 0; y < _IconH; y++) {
                for(int x = iconSrcW; x < _IconW; x++) {
                    int index = y * bytesPerRow + x / 4;
                    _IconData[index] = _IconData[index] & ~(1 << (6 - (x % 4) * 2));
                    _IconSrcData[index] = _IconData[index] & ~(1 << (6 - (x % 4) * 2));
                }
            }
        }
    }
    double horScale = 530 / (double)(_IconW * 3);
    double verScale = 500 / (double)(_IconH * 3);
    _scale = qMin(horScale, verScale);
    _currOrigin.setX((630 / _scale - _IconW * 3) / 2);
    _currOrigin.setY((600 / _scale - _IconH * 3) / 2);
    update();
}

void IconViewEdit::SetIconIndex(int iconIndex)
{
    if(iconIndex < _IconCnt) {
        _IconIndex = iconIndex;
        update();
    }
}

void IconViewEdit::ResetView(void)
{
    memcpy_s(_IconData, ICON_DATA_MAX_SIZE, _IconSrcData, ICON_DATA_MAX_SIZE);
    update();
}

void IconViewEdit::WriteBack(uint8_t *data, uint32_t len)
{
    if(data == nullptr || len != _DataSize) {
        return;
    }
    data[2] = _IconW;
    memcpy_s(data + 6, len - 6, _IconData, _DataSize - 6);
}

void IconViewEdit::SetTransColor(QColor transColor)
{
    _transColor = transColor;
}

void IconViewEdit::LoadImageBinary(QImage image, BinAlg binAlg)
{
    QImage rsImgRgb = image.scaled(_IconW, _IconH).convertToFormat(QImage::Format_RGB32);
    QImage rsImg    = image.scaled(_IconW, _IconH).convertToFormat(QImage::Format_Grayscale8);

    for(int y = 0; y < _IconH; y++) {
        for(int x = 0; x < _IconW; x++) {
            QRgb rgb = rsImgRgb.pixel(x, y);
            if(qAbs(qRed(rgb) - _transColor.red()) < 5 && qAbs(qGreen(rgb) - _transColor.green()) < 5 && qAbs(qBlue(rgb) - _transColor.blue()) < 5) {
                rsImg.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }

    uint8_t *grayImg = new uint8_t[_IconW * _IconH];
    if(grayImg == nullptr) {
        return;
    }

    for(int y = 0; y < rsImg.height(); y++) {
        for(int x = 0; x < rsImg.width(); x++) {
            grayImg[y * rsImg.width() + x] = qGray(rsImg.pixel(x, y));
        }
    }

    uint8_t *binImg = new uint8_t[_IconW * _IconH];
    if(binImg == nullptr) {
        delete[] grayImg;
        return;
    }

    if(binAlg == BinAlg_Otsu) {
        unsigned char thresh = calc_otsu(grayImg, _IconW, _IconH);
        threshold(grayImg, binImg, _IconW, _IconH, thresh);
    }
    else if(binAlg == BinAlg_Wellner) {
        adaptive_threshold_S(grayImg, binImg, _IconW, _IconH, 5);
    }
    else {
        memset(binImg, 0, _IconW * _IconH);
    }

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        dataPtr += _IconW * _IconH * _IconTransparency / 8 * _IconIndex;
    }
    for(int y = 0; y < _IconH; y++) {
        for(int x = 0; x < _IconW; x++) {
            if(_IconTransparency < 2) {
                int bytesPerRow = _IconW / 8;
                int index = y * bytesPerRow + x / 8;
                if(binImg[y * _IconW + x]) {
                    dataPtr[index] = dataPtr[index] | (1 << (7 - (x % 8)));
                } else {
                    dataPtr[index] = dataPtr[index] & ~(1 << (7 - (x % 8)));
                }
            }
            else if(_IconTransparency < 3) {
                int bytesPerRow = _IconW / 4;
                int index = y * bytesPerRow + x / 4;
                QRgb rgb = rsImgRgb.pixel(x, y);
                if(qAbs(qRed(rgb) - _transColor.red()) < 5 && qAbs(qGreen(rgb) - _transColor.green()) < 5 && qAbs(qBlue(rgb) - _transColor.blue()) < 5) {
                    dataPtr[index] = dataPtr[index] |  (1 << (7 - (x % 4) * 2));
                    dataPtr[index] = dataPtr[index] & ~(1 << (6 - (x % 4) * 2));
                } else {
                    if(binImg[y * _IconW + x]) {
                        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (x % 4) * 2));
                        dataPtr[index] = dataPtr[index] |  (1 << (6 - (x % 4) * 2));
                    } else {
                        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (x % 4) * 2));
                        dataPtr[index] = dataPtr[index] & ~(1 << (6 - (x % 4) * 2));
                    }
                }
            }
        }
    }

    update();

    delete[] grayImg;
    delete[] binImg;
}

void IconViewEdit::SetBrushType(BrushType brushType)
{
    _brushType = brushType;
}

void IconViewEdit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawRect(rect());

    painter.scale(_scale, _scale);
    painter.translate(_currOrigin);

    painter.setBrush(QBrush(Qt::gray, Qt::BrushStyle::Dense4Pattern));
    painter.drawRect(0, 0, _IconW * 3, _IconH * 3);

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        dataPtr += _IconW * _IconH * _IconTransparency / 8 * _IconIndex;
    }
    if(_IconTransparency < 2) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 8); i++) {
                uint8_t data = dataPtr[y * _IconW / 8 + i];
                for(int j = 0; j < 8; j++) {
                    if(data & (1 << (7 - j))) {
                        painter.setBrush(Qt::black);
                    } else {
                        painter.setBrush(Qt::white);
                    }
                    painter.drawRect((i * 8 + j) * 3, y * 3, 3, 3);
                }
            }
        }
    }
    else if(_IconTransparency < 3) {
        for(int y = 0; y < _IconH; y++) {
            for(int i = 0; i < (_IconW / 4); i++) {
                uint8_t data = dataPtr[y * _IconW / 4 + i];
                for(int j = 0; j < 4; j++) {
                    if((data & (1 << ((7 - j * 2)))) == 0) {
                        if(data & (1 << ((6 - j * 2)))) {
                            painter.setBrush(Qt::black);
                        } else {
                            painter.setBrush(Qt::white);
                        }
                        painter.drawRect((i * 4 + j) * 3, y * 3, 3, 3);
                    }
                }
            }
        }
    }
}

void IconViewEdit::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());
    if(event->button() == Qt::LeftButton) {
        switch(_brushType)
        {
        case BrushType_Black:
            emit posBlack(pressedPos);
            break;

        case BrushType_White:
            emit posWhite(pressedPos);
            break;

        case BrushType_Transparent:
            emit posTransparent(pressedPos);
            break;

        default:
            break;
        }
    }
    else if(event->button() == Qt::RightButton) {
        if(_IconTransparency < 2) {
            _brushType = static_cast<BrushType>(((int)_brushType + 1) % 2);
        }
        else if(_IconTransparency < 3) {
            _brushType = static_cast<BrushType>(((int)_brushType + 1) % 3);
        }
        emit brushTypeChanged(_brushType);
    }
    else if(event->button() == Qt::MiddleButton) {
        _startPoint = event->pos();
    }

    update();
}

void IconViewEdit::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pressedPos(event->pos().x() / _scale - _currOrigin.x(), event->pos().y() / _scale - _currOrigin.y());
    if(event->buttons() & Qt::LeftButton) {
        switch(_brushType)
        {
        case BrushType_Black:
            emit posBlack(pressedPos);
            break;

        case BrushType_White:
            emit posWhite(pressedPos);
            break;

        case BrushType_Transparent:
            emit posTransparent(pressedPos);
            break;

        default:
            break;
        }
    }
    else if(event->buttons() & Qt::MiddleButton) {
        QPoint endPoint = event->pos();
        _currOrigin = (endPoint - _startPoint) / _scale + _prevOrigin;
    }

    update();
}

void IconViewEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton) {
        _prevOrigin = _currOrigin;
    }

    update();
}

void IconViewEdit::wheelEvent(QWheelEvent *event)
{
    QPoint prevMousePos = event->position().toPoint() / _scale - _currOrigin;
    if(event->angleDelta().y() > 0)
    {
        _scale += _scale / 10;
        QPoint currMousePos = event->position().toPoint() / _scale - _currOrigin;
        _currOrigin = _currOrigin + currMousePos - prevMousePos;
        _prevOrigin = _currOrigin;
    }
    else if(_scale > 0.005)
    {
        _scale -= _scale / 10;
        QPoint currMousePos = event->position().toPoint() / _scale - _currOrigin;
        _currOrigin = _currOrigin + currMousePos - prevMousePos;
        _prevOrigin = _currOrigin;
    }

    update();
}

void IconViewEdit::on_posBlack(QPoint pos)
{
    if(!QRect(0, 0, _IconW * 3, _IconH * 3).contains(pos)) {
        return;
    }

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        dataPtr += _IconW * _IconH * _IconTransparency / 8 * _IconIndex;
    }
    if(_IconTransparency < 2) {
        int bytesPerRow = _IconW / 8;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 8;
        dataPtr[index] = dataPtr[index] | (1 << (7 - (pos.x() / 3 % 8)));
    }
    else if(_IconTransparency < 3) {
        int bytesPerRow = _IconW / 4;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 4;
        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (pos.x() / 3 % 4) * 2));
        dataPtr[index] = dataPtr[index] |  (1 << (6 - (pos.x() / 3 % 4) * 2));
    }

    update();
}

void IconViewEdit::on_posWhite(QPoint pos)
{
    if(!QRect(0, 0, _IconW * 3, _IconH * 3).contains(pos)) {
        return;
    }

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        dataPtr += _IconW * _IconH * _IconTransparency / 8 * _IconIndex;
    }
    if(_IconTransparency < 2) {
        int bytesPerRow = _IconW / 8;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 8;
        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (pos.x() / 3 % 8)));
    }
    else if(_IconTransparency < 3) {
        int bytesPerRow = _IconW / 4;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 4;
        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (pos.x() / 3 % 4) * 2));
        dataPtr[index] = dataPtr[index] & ~(1 << (6 - (pos.x() / 3 % 4) * 2));
    }

    update();
}

void IconViewEdit::on_posTransparent(QPoint pos)
{
    if(!QRect(0, 0, _IconW * 3, _IconH * 3).contains(pos)) {
        return;
    }

    uint8_t *dataPtr = _IconData;
    if(_IconIndex < _IconCnt) {
        dataPtr += _IconW * _IconH * _IconTransparency / 8 * _IconIndex;
    }
    if(_IconTransparency < 2) {
        int bytesPerRow = _IconW / 8;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 8;
        dataPtr[index] = dataPtr[index] & ~(1 << (7 - (pos.x() / 3 % 8)));
    }
    else if(_IconTransparency < 3) {
        int bytesPerRow = _IconW / 4;
        int index = pos.y() / 3 * bytesPerRow + pos.x() / 3 / 4;
        dataPtr[index] = dataPtr[index] |  (1 << (7 - (pos.x() / 3 % 4) * 2));
        dataPtr[index] = dataPtr[index] & ~(1 << (6 - (pos.x() / 3 % 4) * 2));
    }

    update();
}
