#include <QtGui>
#include "add_a_peer.h"
#include "ui_add_a_peer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"




#include <parser.h>
#include <state.h>
#include <net.h>


void PGP_LOG_PAGE_DESCRIPTOR::on_Generate_Key_returnPressed()//the button gererates a new key
{
//TODO complicated and not going to implement for this demo
}
void PGP_LOG_PAGE_DESCRIPTOR::on_Export_Key_returnPressed()//exports the key into a new destination
{

}
void PGP_LOG_PAGE_DESCRIPTOR::on_Load_Key_returnPressed()//Loads your most current key
{
    //This does not seem right...
((MainWindow *)this->parent)->crypto.setPrivateKey(Key_Box().toStdString());
    ui-> Key_Box -> clear();
}
