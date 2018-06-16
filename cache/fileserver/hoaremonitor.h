/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : hoaremonitor.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un moniteur de Hoare

 Commentaires : Prise du cours au slide 31 et 33 de moniteur
 -----------------------------------------------------------------------------
*/

#ifndef HOAREMONITOR_H
#define HOAREMONITOR_H

#include <QSemaphore>

class HoareMonitor {
protected:
    class Condition {
        friend HoareMonitor;
    public:
        Condition();
    private:
        QSemaphore waitingSem;
        int nbWaiting;
    };

public:
    HoareMonitor();

    void monitorIn();

    void monitorOut();

    void wait(Condition &cond);

    void signal(Condition &cond);
private:
    QSemaphore monitorMutex;
    QSemaphore monitorSignale;
    int monitorNbSignale;
};

#endif // HOAREMONITOR_H
