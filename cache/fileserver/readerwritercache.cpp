/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : readerwritercache.cpp
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe implémentant un cache

 Commentaires : Base donnée dans le labo
 -----------------------------------------------------------------------------
*/
#include "readerwritercache.h"
#include "option.h"
#include <QDateTime>
#include <QList>
#include <QDebug>

ReaderWriterCache::ReaderWriterCache(int invalidationDelaySec, int staleDelaySec):
    invalidationDelaySec(invalidationDelaySec), staleDelaySec(staleDelaySec)
{
    timer = new InvalidationTimer(this);
    timer->start();
}

ReaderWriterCache::~ReaderWriterCache(){

    timer->requestInterruption();
    timer->wait();
    delete timer;
}

void ReaderWriterCache::putResponse(Response &response) {
    lock.lockWriting();

    TimestampedResponse res(response, timer->getTimer());

    //On ajoute la requête travaillée dans le cache
    map.insert(response.getRequest().getFilePath(), res);

    lock.unlockWriting();
}

Option<Response> ReaderWriterCache::tryGetCachedResponse(Request &request) {
    lock.lockReading();

    //On regarde dans le cache si la requête existe
    const QHash<QString, TimestampedResponse>::iterator i = map.find(request.getFilePath());

    Option<Response> res = Option<Response>::none();

    //Si elle existe, on la récupère et on met à jour son timestamp
    if(i != map.end()){
        res = Option<Response>::some(i->response);
        i->timestamp_s = timer->getTimer();
    }
    lock.unlockReading();
    return res;
}
