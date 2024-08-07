#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <QDebug>
#include <QWidget>
#include <QPainter>

#define ICON_DATA_MAX_SIZE  1048576

class IconView : public QWidget
{
    Q_OBJECT
public:
    explicit IconView(QWidget *parent = nullptr);
    void ClearView(void);
    void UpdateView(uint8_t *data, uint32_t len);
    void SetIconIndex(int iconIndex);

    int32_t GetIconWidth(void) {
        return _IconW;
    }

    int32_t GetIconHeight(void) {
        return _IconH;
    }

    int32_t GetIconCount(void) {
        return _IconCnt;
    }

    int32_t GetIconTransparency(void) {
        return _IconTransparency;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal   _scale;
    int32_t _IconW;
    int32_t _IconH;
    int32_t _IconCnt;
    int32_t _IconTransparency;
    uint8_t _IconData[ICON_DATA_MAX_SIZE];
    uint8_t _IconIndex;
    int32_t _DataSize;

};

#endif // ICONVIEW_H
