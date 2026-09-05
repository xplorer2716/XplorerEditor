# Analyse : implémentation MIDI de Xplorer vs. spécification Oberheim Xpander/Matrix-12

Comparaison de la couche MIDI/SysEx de XplorerEditor avec la spécification de référence
([xplorer2716/OberheimXpanderMidiSpec](https://github.com/xplorer2716/OberheimXpanderMidiSpec), `README.md`,
sections "Appendix A: MIDI Implementation Summary" et "Appendix B: System Exclusive Information").

## Périmètre couvert par cette analyse

Xplorer, dans son état actuel, **ne gère pas l'édition des multi-patches** — seuls les single patches
(voice data) sont édités en temps réel. Le multi-patch n'intervient que via les opérations globales de
sauvegarde/restauration ("All data dump request" / backup-restore de la mémoire complète), pas via une
UI d'édition dédiée.

En conséquence, plusieurs écarts identifiés par une première passe d'analyse concernaient en réalité des
commandes SysEx qui n'ont de sens qu'en contexte multi-patch (sélection de voix, copie inter-voix,
sélection de carte sur Matrix-12) : ils sont documentés plus bas comme **hors périmètre actuel**, pas
comme des bugs, et ne justifient pas d'issue tant que l'édition multi-patch n'est pas au programme.

Fichiers de référence côté implémentation :
`juce/controller/src/XpanderController.cpp`, `juce/controller/src/XpanderControllerMidiEvents.cpp`,
`juce/model/include/xplorer/model/XpanderConstants.hpp`,
`juce/model/include/xplorer/model/XpanderSinglePatch.hpp`.

## Conformité confirmée

- Format générique des trames `F0 10 <device> <cmd> ... F7` respecté partout.
- Device ID : `02H` (Xpander) partout, sauf `04H` réservé au dump multi-patch Matrix-12
  (`isMultiPatchProgramDumpSysex`, `XpanderControllerMidiEvents.cpp:466-468`) — conforme à la Note 3 de
  la spec ("both use Device Number 02H except during Multi Patch data dumps when the Matrix-12 uses 04H").
- Program data dump request/follows, All data dump request (+ variante Matrix-12), Page/Subpage select,
  Store (émission), Display control command (`05H`/`06H` selon le type de synthé sélectionné),
  Programmer mode switches (émission "SINGLE") : conformes octet à octet à l'Annexe B.
- Format binaire du patch (`XpanderSinglePatch.hpp` : intro `F0 10 02 01 00`, 196 octets *packetized*
  sur 2 octets chacun) conforme à la section "Bulk Data Format".

## Gap réel identifié

### "Page edit follows" : seul le premier triplet d'une trame groupée est décodé

→ [Issue #80](https://github.com/xplorer2716/XplorerEditor/issues/80)

La spec (section EOX) précise qu'une trame *Page edit follows* peut contenir **jusqu'à 6 triplets
consécutifs** `<id> <rot/val>` dans un seul message SysEx (ex. rotation rapide d'un encodeur en face
avant). `isPageEditFollowsSysex` (`XpanderControllerMidiEvents.cpp:500-541`) et la constante
`XpanderParameter::SYSEX_MESSAGE_LENGTH = 12` ne traitent qu'un unique triplet par trame ; un message plus
long passe le test de garde (`size() < 12`) mais les triplets au-delà du premier sont silencieusement
ignorés.

Ce point est indépendant du support multi-patch : *Page edit follows* est utilisé pour toutes les pages
d'édition (VCO, VCF, ENV, LFO, ...), y compris en single patch — c'est pourquoi il est traité comme un
gap actionnable dès maintenant, contrairement aux points listés dans la section suivante.

## Écarts hors périmètre actuel (multi-patch non géré)

Ces commandes de l'Annexe B ne sont ni émises ni interprétées à la réception, mais elles ne s'appliquent
qu'à des scénarios multi-patch/multi-voix que Xplorer n'édite pas aujourd'hui. Documentées ici pour
mémoire, sans issue associée :

- **Copy (`04H`)** — "may only be used in multi patch mode when a single voice is currently selected"
  (copie du buffer d'édition d'une voix vers d'autres voix). Sans issue tant que l'édition multi-patch
  n'existe pas.
- **Programmer mode switches en réception (`0DH`)** — permettrait de détecter un changement SINGLE/MULTI
  ou une sélection de voix (1/7, 2/8, ...) initié depuis la face avant. Pertinent seulement si Xplorer
  doit un jour refléter l'état multi-patch du synthé.
- **Card select, Matrix-12 uniquement (`10H`)** — sélection de la carte de voix (1-6 vs 7-12) sur
  Matrix-12, utile pour l'édition de voix individuelles dans un contexte multi-patch/12 voix.

## Couverture partielle (à évaluer séparément, non urgent)

- **Transpose (`0CH`)** — non implémenté (ni émis ni reçu). C'est un réglage global (Master Transpose),
  indépendant du multi-patch, mais probablement d'un intérêt limité pour l'édition de patch — à évaluer
  si des utilisateurs le demandent.
- **Store (`07H`) et Programmer mode switches (`0DH`) en réception** — le synthé les émet lorsqu'une
  action est faite en face avant (Annexe B, "Transmitted Data"), mais Xplorer ne les interprète pas au
  retour. Pour *Store*, l'impact en single patch reste limité (pas de changement d'état à répercuter côté
  éditeur). Non traité en issue pour l'instant, faute de scénario utilisateur clair en single patch.
- **Program data dump request toujours en "voice data"** — `sendProgramDumpRequestToSynth`
  (`XpanderController.cpp:799-808`) fixe systématiquement le "Command byte 2" à `00H` (voice data). Il
  n'existe aucun moyen de demander explicitement un multi-patch précis par numéro
  (`0ccc cccc > 0`) ; seul le dump global ramène les multi-patches. Cohérent avec le périmètre actuel
  (éditeur single-patch), à revisiter si l'édition multi-patch est un jour ajoutée.

## Quirks hérités du portage C# → C++ (comportement préservé intentionnellement)

Deux écarts par rapport au texte de la spec sont présents dans le code, documentés en commentaire comme
volontairement conservés pour fidélité avec la version de référence (C#). Ils ne sont pas nécessairement
des bugs — le firmware Xpander/Matrix-12 les tolère peut-être sans effet — mais méritent vérification :

- **`0xF0` dupliqué dans le message "Programmer Mode Single Patch"**
  (`XpanderController::sendProgrammerModeSinglePatch`, `XpanderController.cpp:381-391`) → trame réellement
  envoyée `F0 F0 10 02 0D 01 00 F7` au lieu de `F0 10 02 0D 01 00 F7`.
  → [Issue #81](https://github.com/xplorer2716/XplorerEditor/issues/81)

- **"Tune Request" (`F6H`) encapsulé dans une enveloppe SysEx** au lieu d'être transmis comme un octet de
  statut System Common isolé (`XpanderController::sendTuneRequestToSynth`, `XpanderController.cpp:646-655`)
  → trame envoyée `F0 F6 F7` au lieu d'un simple octet `F6`.
  → [Issue #82](https://github.com/xplorer2716/XplorerEditor/issues/82)

## Résumé

Le cœur du protocole utile au périmètre actuel de Xplorer (dump de patch simple, sélection de page,
édition de page, matrice de modulation, all-data-dump global) est fidèlement porté. Le seul gap
actionnable dès maintenant est la troncature des trames *page edit follows* multi-triplets (#80). Les
deux quirks hérités (#81, #82) méritent une vérification sur matériel réel avant correction, pour ne pas
casser un comportement dont on ignore s'il est accidentellement nécessaire. Les autres écarts relevés
sont hors périmètre tant que l'édition multi-patch n'est pas implémentée.
