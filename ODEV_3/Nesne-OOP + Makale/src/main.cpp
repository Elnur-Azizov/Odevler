#include <QApplication>
#include "AnaPencere.h"

int main(int argc, char* argv[])
{
    QApplication uygulama(argc, argv);

    AnaPencere pencere;
    pencere.show();

    return uygulama.exec();
}
