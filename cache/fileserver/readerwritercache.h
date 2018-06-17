/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : readerwritercache.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un cache

 Commentaires : Base donnée dans le labo, pour facilité la correction, tout est dans le .h
 -----------------------------------------------------------------------------
*/

#ifndef READERWRITERCACHE_H
#define READERWRITERCACHE_H
#include <QThread>
#include <QMutex>
#include <QHash>

#include "readerwriterlock.h"
#include "response.h"
#include "request.h"
#include "option.h"

class ReaderWriterCache
{
private:
    struct TimestampedResponse {
        Response response;
        unsigned long long  timestamp_s; //renommage, car c'est en seconde
        TimestampedResponse(Response response, long timestamp_s) : response(response), timestamp_s(timestamp_s){
        }
    };

    class InvalidationTimer: public QThread {
        friend ReaderWriterCache;

    private:
        ReaderWriterCache* cache;
        unsigned long long timer_s;

    public:
        InvalidationTimer(ReaderWriterCache* cache): cache(cache), timer_s(0) {
        }

        unsigned long long getTimer(){
            return timer_s;
        }
    protected:
        void run() {

            while(true){

                QHash<QString, TimestampedResponse>::iterator it;
                QHash<QString, TimestampedResponse>::iterator begin  = cache->map.begin();
                QHash<QString, TimestampedResponse>::iterator end = cache->map.end();

                //Si le serveur tourne depuis 593 milliard d'années, on remet tout à zéro


                it = begin;
                while(it != end){
                    if((timer_s - it.value().timestamp_s) >= (unsigned long long)cache->staleDelaySec){
                        it = cache->map.erase(it);
                    }else{
                        it++;
                    }
                }

                //Tous les invalidationDelaySec on vérifie le tout
                QThread::sleep(cache->invalidationDelaySec);
                timer_s += cache->invalidationDelaySec;

                if(isInterruptionRequested()){
                    break;
                }
            }
        }
    };

    QHash<QString, TimestampedResponse> map;
    int invalidationDelaySec;
    InvalidationTimer* timer;
    ReaderWriterLock lock;
    int staleDelaySec;

public:
    ReaderWriterCache(int invalidationDelaySec, int staleDelaySec):
        invalidationDelaySec(invalidationDelaySec), staleDelaySec(staleDelaySec)
    {
        timer = new InvalidationTimer(this);
        timer->start();
    }

    ~ReaderWriterCache(){

        timer->requestInterruption();
        timer->wait();
        delete timer;
    }

    void putResponse(Response &response) {
        lock.lockWriting();

        TimestampedResponse timRes(response, timer->getTimer());

        //On ajoute la requête travaillée dans le cache
        map.insert(response.getRequest().getFilePath(), timRes);

        lock.unlockWriting();
    }

    Option<Response> tryGetCachedResponse(Request &request) {
        lock.lockReading();

        //On regarde dans le cache si la requête existe
        const QHash<QString, TimestampedResponse>::iterator i = map.find(request.getFilePath());

        Option<Response> response = Option<Response>::none();

        //Si elle existe, on la récupère et on met à jour son timestamp
        //Car s'il est lu, c'est qu'il peut "vivre" plus longtemps
        if(i != map.end()){
            response = Option<Response>::some(i->response);
            i->timestamp_s = timer->getTimer();
        }
        lock.unlockReading();
        return response;
    }

};

#endif // READERWRITERCACHE_H
