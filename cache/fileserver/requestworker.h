/*
 -----------------------------------------------------------------------------
 Labo        : 6 - Server
 Fichier     : requestworker.h
 Auteur(s)   : David Jaquet et Vincent Guidoux
 Date        : 22.05.2018

 But         : définission et implémentation du thread de traitement
               de requête.
 -----------------------------------------------------------------------------
*/
#include "requesthandler.h"
#include "response.h"
#include "readerwritercache.h"
#include "option.h"

#ifndef REQUESTWORKER_H
#define REQUESTWORKER_H

#endif // REQUESTWORKER_H


class RequestWorker : public QThread
{
private:
    Request request;
    AbstractBuffer<Response>* responses;
    ReaderWriterCache* cache;
    bool hasDebugLog;

public:
    RequestWorker(Request request,
                  AbstractBuffer<Response>* responses,
                  ReaderWriterCache* cache,
                  bool hasDebugLog):
        request(request), responses(responses), cache(cache), hasDebugLog(hasDebugLog) {}

protected:

    /**
     * @brief run : traîte une requête et la place dans le tampon de
     *              réponses
     */
    void run (){
        Response resp;
        Option<Response> cachedResponse = cache->tryGetCachedResponse(request);
        if (cachedResponse.hasValue()) {
            resp = cachedResponse.value();
        } else {
            resp = RequestHandler(request, hasDebugLog).handle();
            cache->putResponse(resp);
        }
        responses->put(resp);
    }

};
