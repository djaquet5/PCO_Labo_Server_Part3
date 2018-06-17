# Serveur - Cache - Etape 3

Auteurs : Vincent Guidoux & David Jaquet

## Description des fonctionnalités du logiciel

L'application est un serveur de fichier qui peut supporter la montée en charge du nombre de clients et de requêtes grâce à de la concurrence.

Le client est une application web qui a été fournie. Cette dernière permet à l'utilisateur de donner le chemin pointant vers un fichier texte et qui affiche son contenu. L'utilisateur peut également modifier le nombre de requête qu'il envoie au serveur.

Le serveur récupère le nom du fichier, lit le fichier et envoie son contenu en sortie. Le serveur peut donc nous gérer plusieurs requête simultanément. Ce nombre est définis par l'utilisateur grâce à l'application web cliente. Un squelette du serveur nous a été fournis.

Le laboratoire est divisé en 3 parties distinctes. Pour cette dernière partie, nous devons faire les tâches suivantes :

- Implémentation de la classe ``ReaderWriterLock``.
- Implémentation de la classe ``ReaderWriterCache``, en incluant un mécanisme de timer pour périodiquement invalider les réponses trop "âgées" (stale data).
- Adaptation du code du dispatcher de requêtes pour exploiter le cache.
- Adaptation du code de ``RequestRunnable`` ou équivalent pour expoilter le cache.

## Choix d'implémentation

Nous n'avons malheureusement pas réussi à produire un code fonctionnel à 100% lors de l'étape précédente. C'est pourquoi nous avons donc pris comme base le code que nous avons rendus lors de l'étape 1. Pour cette dernière partie, nous avons implémenté avec un **moniteur de Hoare**.

Nous avons donc commencé par l'implémentation de la classe ``HoareMonitor`` qui était une chose aisée car nous avions déjà une implémentation fournie dans le cours.

Nous avons décidé de ne pas ré-initialiser le timer d'invaladition car il nous faudrait environ 584 milliards d'années avant d'arriver à un bug. Notre timer utilise compte avec des ``unsigned long long`` et, en ``C++``, les ``unsigned long long`` peuvent atteindre la valeur de `` 18446744073709551615``. Le nombre d'année est obtenue en divisant ce chiffre par ``3600 * 24 * 365.25``. 

Nous avons choisi de donner la priorité au rédacteurs, car ils est plus important d'avoir les fichiers déjà utilisés que d'essayer de vérifier s'il y en a déjà. Pour nous en tout cas.

###Adaptation du code du dispatcher de requêtes pour exploiter le cache.

```c++
void run()
    {
        while(true) {
            if (hasDebugLog)
                qDebug() << "Waiting for request...";

            tmp =  new RequestWorker(requests->get(),
                                     responses,
                                     cache, // Nous lui passons le pointeur du cache
                                     hasDebugLog);
            tmp->start();
        }
    }
```

###Adaptation du code de ``RequestRunnable`` ou équivalent pour expoilter le cache.

```c++
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
```

## Le cache améliore-t-il la performance et dans quels cas ?

Le cache améliore la performance lorsqu'on a un rapport entre le nombre de fichiers et le nombre de requêtes proche de 0.

## Y-a-t-il des schémas d’utilisation qui pourraient être problématiques avec votre implémentation ? Si oui, commentez lesquels et quelles seraient les mesures à prendre pour y palier. 

Un schéma d'utilisation problématique serait l'utilisation de logiciel avec un grand nombre de fichiers différents. Notre implémentation va devoir prendre du temps d'écrire chaque fichier et, tant que nous avons des fichiers à écrire, les lecteurs seront en attente tant que le rédacteur n'aura pas terminé leur tâche. Dans notre cas, nous n'avons pas ce problème car nous utilisons uniquement le fichier ``Shakespeare.txt``.

Une solution envisageable serait de changer la priorité pour n'en avoir aucune

## Tests effectués

Étant donné que nous n'avons pu utiliser le ThreadPool de l'étape 2. Nos tests ne sont pas très concluant. Effectivement pour 1000 requêtes, 1000 threads sont créés au même moment et le serveur plante.  Du coup, aucune amélioration n'est remarquée, c'est assez frustrant, mais le débug de la deuxièmepartie prenait trop de temps. 