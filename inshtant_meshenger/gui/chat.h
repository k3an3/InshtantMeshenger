#ifndef CHAT_H
#define CHAT_H

#include <QChat>

namespace Ui {
class Chat;
}

class Chat : public QChat
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = 0);
    ~Chat();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
