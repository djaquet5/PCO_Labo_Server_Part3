/*
 -----------------------------------------------------------------------------
 Labo        : 8 - Server - étape 3
 Fichier     : readerwritercache.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 16.06.2018

 But         : Classe définissant un cache

 Commentaires : Base donnée dans le labo
 -----------------------------------------------------------------------------
*/

#ifndef READERWRITERCACHE_H
#define READERWRITERCACHE_H
#include <QHash>
#include <QThread>
#include <QMutex>
#include <QTime>
#include "option.h"
#include "request.h"
#include "response.h"
#include "readerwriterlock.h"
#include <iostream>

class ReaderWriterCache
{
private:
    struct TimestampedResponse {
        Response response;
        long timestamp_s; //renommage, car c'est en seconde
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
                std::cout << "cunni";

                QHash<QString, TimestampedResponse>::iterator it;
                QHash<QString, TimestampedResponse>::iterator begin  = cache->map.begin();
                QHash<QString, TimestampedResponse>::iterator end = cache->map.end();

                if(timer_s == ULLONG_MAX){
                    //Si le erveur tourne depuis 593 milliard d'années, on remet tout à zéro
                    for(it = begin; it != end; it++){
                        it->timestamp_s = 0;
                    }
                    timer_s = 0;
                }


                it = begin;
                while(it != end){
                    if((timer_s - it.value().timestamp_s) >= cache->staleDelaySec){
                        it = cache->map.erase(it);
                    }else{
                        it++;
                    }
                }

                //Tout les invalidationDelaySec on vérifie le tout
                QThread::sleep(cache->invalidationDelaySec);
                timer_s += cache->invalidationDelaySec;
                // timer_s = timer_s < 0 ? ULLONG_MAX : timer_s;

                if(isInterruptionRequested()){
                    break;
                }
            }
        }
    };

    QHash<QString, TimestampedResponse> map;
    int invalidationDelaySec;
    int staleDelaySec;
    InvalidationTimer* timer;
    ReaderWriterLock lock;

public:
    ReaderWriterCache(int invalidationDelaySec, int staleDelaySec);
    ~ReaderWriterCache();
    Option<Response> tryGetCachedResponse(Request& request);
    void putResponse(Response& response);
};

#endif // READERWRITERCACHE_H
