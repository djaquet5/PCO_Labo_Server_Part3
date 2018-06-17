/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : readerwriterlock.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un lecteur-rédacteur avec priorité au rédacteur

 Commentaires : Priorité au rédacteurs
 -----------------------------------------------------------------------------
*/

#ifndef READERWRITERLOCK_H
#define READERWRITERLOCK_H

#include "hoaremonitor.h"

class ReaderWriterLock : public HoareMonitor
{
private:
    int nbReaders, nbWaitingReaders, nbWaitingWriters;
    Condition condReader, condWriter;
    bool isWriting;
public:
    ReaderWriterLock() :
        nbReaders(0), nbWaitingReaders(0), nbWaitingWriters(0), isWriting(false) {}

    void lockReading() {
        monitorIn();
        if(isWriting || (nbWaitingWriters > 0)){
            nbWaitingReaders++;
            wait(condReader);
        }else{
            nbReaders++;
        }
        monitorOut();
    }

    void unlockReading() {
        monitorIn();
        nbReaders--;
        if(nbReaders == 0 && nbWaitingWriters > 0){
            isWriting = true;
            nbWaitingWriters--;
            signal(condWriter);
        }
        monitorOut();
    }

    void lockWriting() {
        monitorIn();
        if(isWriting) {
            nbWaitingWriters++;
            wait(condWriter);
        }else{
            isWriting = true;
        }
        monitorOut();
    }

    void unlockWriting() {
        monitorIn();
        isWriting = false;

        if(nbWaitingReaders == 0 && nbWaitingWriters > 0){
            isWriting = true;
            nbWaitingWriters--;
            signal(condWriter);

        }else{
            for(int i = 0; i < nbWaitingReaders; i++){
                signal(condReader);
            }
            nbReaders = nbWaitingReaders;
            nbWaitingReaders = 0;
        }
        monitorOut();
    }

};

#endif // READERWRITERLOCK_H
