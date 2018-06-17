/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : readerwriterlock.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un lecteur-rédacteur avec priorité au rédacteur

 Commentaires : Base donnée dans le labo
 -----------------------------------------------------------------------------
*/
#include "readerwriterlock.h"

ReaderWriterLock::ReaderWriterLock() :
    nbReaders(0), nbWaitingReaders(0), nbWaitingWriters(0), isWriting(false) {}

void ReaderWriterLock::lockReading() {
    monitorIn();
    if(isWriting || (nbWaitingWriters > 0)){
        nbWaitingReaders++;
        wait(condReader);
    }else{
        nbReaders++;
    }
    monitorOut();
}

void ReaderWriterLock::unlockReading() {
    monitorIn();
    nbReaders--;
    if(nbReaders == 0 && nbWaitingWriters > 0){
        isWriting = true;
        nbWaitingWriters--;
        signal(condWriter);

    }
    monitorOut();
}

void ReaderWriterLock::lockWriting() {
    monitorIn();
    if(isWriting) {
        nbWaitingWriters++;
        wait(condWriter);
    }else{
        isWriting = true;
    }
    monitorOut();
}

void ReaderWriterLock::unlockWriting() {
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
