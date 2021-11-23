//#########################################################
//#
//# Titre : 	Utilitaires CVS LINUX Automne 21
//#				SIF-1015 - Système d'exploitation
//#				Université du Québec à Trois-Rivières
//#            VERSION CONCURRENTE
//#
//# Auteur : 	Francois Meunier
//#	Date :		Novembre 2021
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "../include/gestionListeChaineeVMS.h"
#include "../include/gestionVMS.h"

//Pointeur de tête de liste
extern struct noeud* head;
//Pointeur de queue de liste pour ajout rapide
extern struct noeud* queue;

// nombre de VM actives
extern int nbVM;

// nombre de threads actifs
extern int nbThreadAELX;
//Semaphores
extern sem_t semH, semQ, semnbVM, semC, semnbThreadAELX;

//#######################################
//#
//# Affiche une série de retour de ligne pour "nettoyer" la console
//#
void cls(void){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}

//#######################################
//#
//# Affiche un messgae et quitte le programme
//#
void error(const int exitcode, const char * message){
	printf("\n-------------------------\n%s\n",message);
	exit(exitcode);
	}
	
/* Sign Extend */
uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

/* Swap */
uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

/* Update Flags */
void update_flags(uint16_t reg[R_COUNT], uint16_t r)
{
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

/* Read Image File */
int read_image_file(uint16_t * memory, char* image_path,uint16_t * origin)
{
	 char fich[200];
	 strncpy(fich, image_path, strlen(image_path)-1);
  	 FILE* file = fopen(fich, "rb");

    if (!file) { return 0; }
    /* the origin tells us where in memory to place the image */
   	*origin=0x3000;

    /* we know the maximum file size so we only need one fread */
    uint16_t max_read = UINT16_MAX - *origin;
    uint16_t* p = memory + *origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);
    /* swap to little endian ???? */
    while (read-- > 0)
    {
    //	printf("\n p * BIG = %x",*p);
       // *p = swap16(*p);
		// printf("\n p * LITTLE = %x",*p);
        ++p;
    }
    return 1;
}


/* Check Key */
uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

/* Memory Access */
void mem_write(uint16_t * memory, uint16_t address, uint16_t val)
{
    memory[address] = val;
}

uint16_t mem_read( uint16_t * memory, uint16_t address)
{
    if (address == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

/* Input Buffering */
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

/* Handle Interrupt */
void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}


void* readTrans(char* nomFichier){
	// Un compteur de thread et une liste de tid
	pthread_t tid[1000];
	int nbThread = 0;
	int i;
	
	
	FILE *f;
	char buffer[100];
	char *tok, *sp;

	//Ouverture du fichier en mode "r" (equiv. "rt") : [r]ead [t]ext
	f = fopen(nomFichier, "rt");
	if (f==NULL)
		error(2, "readTrans: Erreur lors de l'ouverture du fichier.");

	//Lecture (tentative) d'une ligne de texte
	fgets(buffer, 100, f);

	//Pour chacune des lignes lues
	while(!feof(f)){
		
		sleep(1);

		//Extraction du type de transaction
		tok = strtok_r(buffer, " ", &sp);

		//Branchement selon le type de transaction
		switch(tok[0]){
			case 'A':
			case 'a':{
				// Creer un thread pour addItem (Ajouter une VM)
				pthread_create(&tid[nbThread++], NULL, addItem, NULL);
				break;
				}
			case 'E':
			case 'e':{
				//Extraction du paramètre
				int noVM = atoi(strtok_r(NULL, " ", &sp));
				// Creer un thread pour removeItem (Supprimer une VM)
				
				//Creer une instance de la structure a passer listItems
				struct paramE *ptr = (struct paramE*) malloc(sizeof(struct paramE));
				ptr->noVM = noVM;
				pthread_create(&tid[nbThread++], NULL, removeItem, ptr);
				break;
				}
			case 'L':
			case 'l':{
				//Extraction des paramètres
				int nstart = atoi(strtok_r(NULL, "-", &sp));
				int nend = atoi(strtok_r(NULL, " ", &sp));
				
				//Creer une instance de la structure a passer listItems
				struct paramL *ptr = (struct paramL*) malloc(sizeof(struct paramL));
				ptr->nstart = nstart;
				ptr->nend = nend;
				
				// Creer un thread pour removeItem (Enlever une VM)
				pthread_create(&tid[nbThread++], NULL, listItems, ptr);				
				
				break;
				}
			case 'X':
			case 'x':{
				//Appel de la fonction associée
				int noVM = atoi(strtok_r(NULL, " ", &sp));
				char *nomfich = strtok_r(NULL, "\n", &sp);
				
				//Creer une instance de la structure a passer pthread_create
				struct paramX *ptr = (struct paramX*) malloc(sizeof(struct paramX));
				ptr->noVM = noVM;
				strcpy(ptr->nomfich,(const char *)nomfich);
				
				//Creer un thread pour executer le code binaire du fichier nomFich sur la VM noVM
				pthread_create(&tid[nbThread++], NULL, executeFile, ptr);
				
				break;
				}
		}

		
		//Lecture (tentative) de la prochaine ligne de texte
		fgets(buffer, 100, f);
	}
	
	//Attendre la fin de tous les transactions
	for(i=0; i<nbThread;i++) {
		pthread_join(tid[i], NULL);
	}
	  		
	
	
	//Fermeture du fichier
	fclose(f);
	//Retour
	return NULL;
}

