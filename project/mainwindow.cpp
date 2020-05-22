#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QString"
#include "stdlib.h"
#include "stdio.h"
#include "iostream"
#include "QStandardItemModel"
#include "QDateTime"
#include "QDebug"
#include "QTimer"
#include "string.h"
#include "unistd.h"
#include "chrono"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Aqui se se diseña el reloj digital con conteo
    ui->setupUi(this);
    QTimer *timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showtime()));
    timer->start();

    // Aqui se extrae y se imprime la Fecha y Hora en tiempo real
    QDateTime datetime=QDateTime::currentDateTime();
    QString datetimetext=datetime.toString();
    ui->date_time->setText(datetimetext);
}

// En esta funcion se muestra el tiempo en funcionamiento como reloj digital
void MainWindow::showtime()
{
    QTime time=QTime::currentTime();

    QString time_text=time.toString("hh : mm : ss");

    if ((time.second() % 2 ) == 0)
    {
        time_text[3] = ' ';
        time_text[8] = ' ';
    }
    //ui->digital_clock->setText(time_text);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_enviar_clicked()
{
    // Se inicializan todas las variables
    int carnet, tvolt, tv, tt, frec_prom=60, vel_prom=1800, vol_prom=120, tem_prom=40, ti, dias, horas, minutos, segundos, mili;
    long now, t, tiemp;
    float frec,vol,vel,tem;

    // Se convierte en String el el carnet digitado
    QString car=ui->carnet->text();
    char numerocarnet[8]=" ";
    for (int i=0;i<7;i++)                       //Aqui se convierte un String en un char
    {
        std::string ca = car.toStdString();
        numerocarnet[i]=ca[i];
    }

    // Se inicializan las variables para calcular las toleracias segun el numero de Carnet
    char t_vol, t_vel1, t_vel2, *t_vel;
    t_vol=numerocarnet[2];
    tvolt=atoi(&t_vol);
    t_vel1=numerocarnet[3];
    t_vel2=numerocarnet[4];
    t_vel=strcat(&t_vel1,&t_vel2);
    t_vol=numerocarnet[2];
    tt=atoi(&numerocarnet[6]);
    tv=atoi(t_vel);

    // Se hace el proceso de comunicacion de los archivos .cpp y .bin
    // para la comunicacion de los procesos
    char *argumentos[3];
    char direccion[46]={"/home/alse/Documents/project/comunicacion.cpp"};
    int pid = fork();
    if (pid==0)
    {
        argumentos[1]=numerocarnet;
        argumentos[2]=NULL;
        argumentos[0]=direccion;
        execv("/home/alse/Documents/project/comunicacion.bin",argumentos);
    }

    // Aqui en este proceso se abre el archivo en .txt y se extraen los datos
    FILE *pointtofile;
    unsigned int File_size;
    char *punteroarchivo;
    pointtofile = fopen("/home/alse/Documents/project/datos.txt","r");
    fseek(pointtofile,0,SEEK_END);
    File_size = ftell(pointtofile);
    rewind(pointtofile);
    punteroarchivo = (char*)malloc(File_size*sizeof(char));
    fread(punteroarchivo,1,File_size,pointtofile);
    rewind(pointtofile);
    fscanf(pointtofile,"%f\n %f\n %f\n %f\n %lu\n",&frec,&vel,&vol,&tem,&tiemp);

    // Aqui se combierten los datos del archivo en .txt a Strings
    QString fre = QString::number(frec);
    QString ve = QString::number(vel);
    QString vo = QString::number(vol);
    QString te = QString::number(tem);
    QString tiem = QString::number(tiemp);

    // Aqui pasamos el tiempo dado de timestamp a Fecha y Hora
    ti=(tiemp/1000);
    long timestamp = ti;
    const time_t rawtime=(const time_t)timestamp;
    time_t curr_time;
    char tim[100];
    tm*curr_tm;
    time(&curr_time);
    curr_tm=localtime(&rawtime);
    strftime(tim, sizeof (tim), "%Y/%m/%d  %H:%M:%S", curr_tm);
    QString qtim(tim);
    ui->fecha->setText(qtim);

    // Aqui se mostramos el tiempo rela en timestamp
    now=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    // Aqui cambiamos el tiempo de timestamp a Tiempo real con fecha y hora
    t=now-tiemp;
    dias=t/86400000;
    horas=t/3600000-dias*24;
    minutos=t/60000-dias*24*60-horas*60;
    segundos=t/1000-dias*24*60*60-horas*60*60-minutos*60;
    mili=t-dias*24*60*60*1000-horas*60*60*1000-minutos*60*1000-segundos*1000;

    // Aqui cambiamos los datos de fecha y hora en Strings
    QString tdias = QString::number(dias);
    QString thoras = QString::number(horas);
    QString tminutos = QString::number(minutos);
    QString tsegundos = QString::number(segundos);
    QString tmili = QString::number(mili);

    bool ok;
    //Se convierte carnet a un valor flotante
    carnet = car.toFloat(&ok);
    // Condicional para que el valor del carnet se el correcto y genere los valores con tolerancias
    if(carnet == 2124475 || carnet == 2143642 || carnet == 2153548)
    {
        ui->carnet->setStyleSheet("background-color: green");
        // SE crea la tabla para los registros del motor
        ui->table_valores->setColumnCount(4);
        QStringList lista_valores;
        lista_valores<<"Temperatura"<<"Voltaje"<<"Velocidad"<<"Frecuencia";
        ui->table_valores->setHorizontalHeaderLabels(lista_valores);
        ui->table_valores->setColumnWidth(0,85);
        ui->table_valores->setColumnWidth(1,70);
        ui->table_valores->setColumnWidth(2,85);
        ui->table_valores->setColumnWidth(3,85);
        ui->table_valores->insertRow(ui->table_valores->rowCount());
        ui->table_valores->setItem(ui->table_valores->rowCount()-1, 0, new QTableWidgetItem(te));
        ui->table_valores->setItem(ui->table_valores->rowCount()-1, 1, new QTableWidgetItem(vo));
        ui->table_valores->setItem(ui->table_valores->rowCount()-1, 2, new QTableWidgetItem(ve));
        ui->table_valores->setItem(ui->table_valores->rowCount()-1, 3, new QTableWidgetItem(fre));

        // Se crea la tabla para los registros del tiempo de operacion
        ui->table_operation_time->setColumnCount(5);
        QStringList lista_time_operation;
        lista_time_operation<<"Dias"<<"Horas"<<"Minutos"<<"Segundos"<<"Mili";
        ui->table_operation_time->setHorizontalHeaderLabels(lista_time_operation);
        ui->table_operation_time->setColumnWidth(0,50);
        ui->table_operation_time->setColumnWidth(1,50);
        ui->table_operation_time->setColumnWidth(2,70);
        ui->table_operation_time->setColumnWidth(3,70);
        ui->table_operation_time->setColumnWidth(4,70);
        ui->table_operation_time->insertRow(ui->table_operation_time->rowCount());
        ui->table_operation_time->setItem(ui->table_operation_time->rowCount()-1, 0, new QTableWidgetItem(tdias));
        ui->table_operation_time->setItem(ui->table_operation_time->rowCount()-1, 1, new QTableWidgetItem(thoras));
        ui->table_operation_time->setItem(ui->table_operation_time->rowCount()-1, 2, new QTableWidgetItem(tminutos));
        ui->table_operation_time->setItem(ui->table_operation_time->rowCount()-1, 3, new QTableWidgetItem(tsegundos));
        ui->table_operation_time->setItem(ui->table_operation_time->rowCount()-1, 4, new QTableWidgetItem(tmili));

        // Se generan las tolerancias de de los valores recibidos con los diferentes carnets
        if(tem>=(tem_prom-tt) && tem<=(tem_prom+tt))
        {
            ui->val_tem->setText(te);
            ui->val_tem->setStyleSheet("background-color: green");
        }else {
            ui->val_tem->setText(te);
            ui->val_tem->setStyleSheet("background-color: red");
        }

        if(vel>=(vel_prom-tv) && vel<=(vel_prom+tv))
        {
            ui->val_vel->setText(ve);
            ui->val_vel->setStyleSheet("background-color: green");
        }else {
            ui->val_vel->setText(ve);
            ui->val_vel->setStyleSheet("background-color: red");
        }

        if(frec>=(frec_prom-(frec_prom*0.05)) && frec<=(frec_prom+(frec_prom*0.05)))
        {
            ui->val_frec->setText(fre);
            ui->val_frec->setStyleSheet("background-color: green");
        }else {
            ui->val_frec->setText(fre);
            ui->val_frec->setStyleSheet("background-color: red");
        }

        if(vol>=(vol_prom-tvolt) && vol<=(vol_prom+tvolt))
        {
            ui->val_vol->setText(vo);
            ui->val_vol->setStyleSheet("background-color: green");
        }else {
            ui->val_vol->setText(vo);
            ui->val_vol->setStyleSheet("background-color: red");
        }        
    }else
    ui->carnet->setStyleSheet("background-color: red");

}

