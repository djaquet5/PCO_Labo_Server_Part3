#include "readerwriterlock.h"

ReaderWriterLock::ReaderWriterLock() :
    nbReaders(0), nbWaitingReaders(0), nbWaitingWriters(0), writer(false) {}

void ReaderWriterLock::lockReading() {
    monitorIn();
    if(writer || (nbWaitingWriters > 0)){
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
    if(nbReaders == 0){
        if(nbWaitingWriters > 0){
            writer = true;
            nbWaitingWriters--;
            signal(condWriter);
        }
    }
    monitorOut();
}

void ReaderWriterLock::lockWriting() {
    monitorIn();
    if(writer) {
        nbWaitingWriters++;
        wait(condWriter);
    }else{
        writer = true;
    }
    monitorOut();
}

void ReaderWriterLock::unlockWriting() {
    monitorIn();
    writer = false;

    if(nbWaitingReaders > 0){
        for(int i = 0; i < nbWaitingReaders; i++)
            signal(condReader);
        nbReaders = nbWaitingReaders;
        nbWaitingReaders = 0;
    }else{
        if(nbWaitingWriters > 0){
            writer = true;
            nbWaitingWriters--;
            signal(condWriter);
        }
    }
    monitorOut();
}
