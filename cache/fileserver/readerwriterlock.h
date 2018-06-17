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
    ReaderWriterLock();
    void lockReading();
    void lockWriting();
    void unlockReading();
    void unlockWriting();
};

#endif // READERWRITERLOCK_H
