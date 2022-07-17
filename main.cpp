#include <QApplication>
#include <QPushButton>
#include <QPixmap>
#include <iostream>
#include <QPainter>
#include <QPaintEvent>
#include <QSlider>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>


class ImageButton : public QPushButton
{
        Q_OBJECT
public:
        ImageButton() = default;
        ImageButton (QWidget *parent);
        void paintEvent(QPaintEvent *e) override;
        QSize sizeHint() const override;
        QSize minimumSizeHint() const override;


public slots:
        void setRed(QString Path);

private:
        QPixmap mCurrentBUttonPixmap;
        QPixmap mButtonRedPixmap;

};

ImageButton::ImageButton(QWidget *parent) {
    setParent(parent);
    setToolTip("STOP!");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mCurrentBUttonPixmap = QPixmap();
    setGeometry(mCurrentBUttonPixmap.rect());
}

void ImageButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawPixmap(e->rect(),mCurrentBUttonPixmap);
}

QSize ImageButton::sizeHint() const
{
    return QSize(800,800);
}

QSize ImageButton::minimumSizeHint() const
{
    return QSize(100,100);
}

void ImageButton::setRed(QString Path)
{
    mButtonRedPixmap = QPixmap(Path);
    mCurrentBUttonPixmap = mButtonRedPixmap;
    update();
}

QImage blurImage(QImage source){
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(source));
    //
    auto *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(2);
    item.setGraphicsEffect(blur);
    scene.addItem(&item);
    QImage result(source.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    scene.render(&painter, QRectF(),QRectF(0,0, source.width(),source.height()));
    return result;
}

void processSingleImage(QString sourceFile, QString destFile){
    auto blured = blurImage(QImage(sourceFile));
    blured.save(destFile);
}


int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QWidget *window = new QWidget;
    QWidget *redButton = new ImageButton(window);
    QSlider *slider = new QSlider(Qt::Horizontal,window);
    slider->setMinimum(0);
    slider->setMaximum(10);
    auto *openFileButton = new QPushButton ("open");
    auto *layout = new QVBoxLayout(window);
    layout->addWidget(redButton);
    layout->addWidget(slider);
    layout->addWidget(openFileButton);
    window->show();
    QString filePath;
    QString dPath;

    QObject::connect(openFileButton, &QPushButton::clicked,[&redButton,&filePath,&slider,&dPath] {
        filePath = QFileDialog::getOpenFileName(nullptr,
                                     "Open jpeg file",
                                     "B://",
                                     "jpeg files (*.jpg, *.JPG)");

        ImageButton *c = dynamic_cast<ImageButton *>(redButton);
        c->setRed(filePath);
        QObject::connect(slider, &QSlider::valueChanged, [&redButton, &slider,&filePath,&dPath](int newValue)
        {
                ImageButton *c = dynamic_cast<ImageButton *>(redButton);
                c->setRed(filePath);
                if(c)
                {
                   if(newValue >= 1){
                        processSingleImage(filePath,filePath);
                        c->setRed(filePath);
                    }
                }
        });
    });

    return app.exec();
}
#include "main.moc"
