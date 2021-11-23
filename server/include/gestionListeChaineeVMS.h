#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

struct infoVM{						
	int		noVM;
	unsigned char 	busy; 
	unsigned short * 	ptrDebutVM;							
	};								 

struct noeudVM{			
	struct infoVM	VM;		
	struct noeudVM		*suivant;	
	// Semaphore pour chaque noeud VM
	sem_t semNoeud;
	};	

//la structure pour le passage des parametres au thread executeFile()
struct paramX{
	int noVM;
	char nomfich[100];
	};
	
//la structure pour le passage des parametres au thread removeItem()
struct paramE{
	int noVM;
	};

//la structure pour le passage des parametres au thread listItem()
struct paramL{
	int nstart;
	int nend;
	};



void cls(void);
void error(const int exitcode, const char * message);

struct noeudVM * findItem(const int no);
struct noeudVM * findPrev(const int no);

void addItem(void * param);
void removeItem(struct paramE* param);
//la structure paramL pour le passage des parametres au thread listItems
void listItems(struct paramL* param);
void saveItems(const char* sourcefname);

//la structure paramX pour le passage des parametres au thread executeFile()
void executeFile(struct paramX* param);

void* readTrans(char* nomFichier);