/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : hoaremonitor.cpp
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un moniteur de Hoare

 Commentaires : Prise du cours au slide 31 et 33 de moniteur
 -----------------------------------------------------------------------------
*/
#include "hoaremonitor.h"

HoareMonitor::Condition::Condition() : waitingSem(0), nbWaiting(0) {}

HoareMonitor::HoareMonitor() : monitorMutex(1), monitorSignale(0),
    monitorNbSignale(0) {}

void HoareMonitor::monitorIn(){
    monitorMutex.acquire();
}

void HoareMonitor::monitorOut(){
    if(monitorNbSignale > 0)
        monitorSignale.release();
    else
        monitorMutex.release();
}

void HoareMonitor::wait(Condition &cond){
    cond.nbWaiting++;
    if(monitorNbSignale > 0)
        monitorSignale.release();
    else
        monitorMutex.release();
    cond.waitingSem.acquire();
    cond.nbWaiting--;
}

void HoareMonitor::signal(Condition &cond){
    if(cond.nbWaiting > 0){
        monitorNbSignale++;
        cond.waitingSem.release();
        monitorSignale.acquire();
        monitorNbSignale--;
    }
}
