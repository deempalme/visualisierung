#ifndef OPTIONBUTTONS_H
#define OPTIONBUTTONS_H

#include <QPushButton>

class OptionButtons : public QPushButton
{
  Q_OBJECT
public:
  OptionButtons(const QString _name, QWidget *_parent = 0, uint _id = 0);
  unsigned int getId();

  QString name;

private:
  unsigned int id;

private slots:
  void emitSignal();

signals:
  void buttonReleased(uint _id);
};

#endif // OPTIONBUTTONS_H
