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

#define HOUR 3600
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

            while(1){
                std::cout << "cunni";

                QHash<QString, TimestampedResponse>::iterator it;
                /*
                if(timer_s == ULLONG_MAX){
                    //Si le erveur tourne depuis 593 milliard d'années, on remet tout à zéro
                    for(it = cache->map.begin(); it != cache->map.end(); it++){
                        it->timestamp_s = 0;
                    }
                    timer_s = 0;
                }*/
                for(it = cache->map.begin(); it != cache->map.end(); ){
                    struct TimestampedResponse value = it.value();

                    if((timer_s - value.timestamp_s) >= cache->staleDelaySec){
                        it = cache->map.erase(it);
                    }else{
                        it++;
                    }
                }

                //Tout les invalidationDelaySec on vérifie
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
