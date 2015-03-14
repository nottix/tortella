**Per generare il Makefile bisogna seguire i seguenti passi:**

  1. Posizionarsi nella radice del progetto;
  1. Eseguire il file `./autogen.sh`;
  1. Eseguire il `./configure` sempre nella radice del progetto
  1. Ora il Makefile è pronto, per compilare basta eseguire `make` dalla radice del progetto.

**Esecuzione Programma**

  * l file di esecuzione si trova nella directory `src`;
  * file di configurazione in `src/conf`;
  * file di init in `src/data`;

**Esempio di esecuzione**

  1. Posizionarsi in `src`;
  1. Eseguire `./tortella ./conf/tortella.conf`
  1. Posizionarsi in `src/temp` (crearla se non esiste);
  1. Eseguire `./tortella ../conf/tortella1.conf ../data/init_data`.

Dedicato al Roscio e a Vibra